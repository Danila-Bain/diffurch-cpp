#pragma once

#include "events.hpp"
#include "events/handlers.hpp"
#include "state.hpp"
#include "stepsize.hpp"
#include "symbolic.hpp"
#include "util/vec.hpp"
#include <boost/preprocessor.hpp>
#include <cstddef>
#include <limits>
#include <tuple>

#include "rk_tables/rk98.hpp"

namespace diffurch {
// This class is intended to be exclusively used
// with Curiously Recurring Template Pattern (CRTP),
// that is in creating other classes like
// ``struct Lorenz : Solver<Lorenz> {...};``,
// that inherit solver with themselves.
template <typename Equation> struct Solver {

  auto get_events() { return std::make_tuple(); }

  template <typename RK = rk98, typename StepsizeControllerT = ConstantStepsize>
  auto
  solution(double initial_time, double final_time,
           StepsizeControllerT stepsize_controller = ConstantStepsize(0.1)) {

    return solution<RK>(initial_time, final_time, stepsize_controller,
                        std::make_tuple(StepEvent(SaveAll<Equation>())));
  }

  template <typename RK = rk98, typename StepsizeControllerT,
            typename AdditionalEventsT>
  auto solution(double initial_time, double final_time,
                StepsizeControllerT stepsize_controller,
                AdditionalEventsT additional_events) {
    auto self = static_cast<Equation *>(this);
    auto rhs = self->get_rhs();
    auto ic = self->get_ic();
    static constexpr size_t n = std::tuple_size<decltype(ic(0.))>::value;

    auto events = Events(std::tuple_cat(self->get_events(), rhs.get_events(),
                                        additional_events));
    // todo: make Events contructor with arbitrary arguments

    auto state = State<RK, decltype(ic)>(initial_time, ic);
    state.t_step = stepsize_controller.initial_stepsize;

    Vec<n> delta_x;
    Vec<n> delta_x_hat;

    auto runge_kutta_step = [&]() {
      for (size_t i = 0; i < RK::s; i++) {
        state.t_curr = state.t_prev + state.t_step * RK::c[i];
        state.x_curr =
            state.x_prev + state.t_step * dot(RK::a[i], state.K_curr, i);
        state.K_curr[i] = rhs(state);
        events.call_events(state);
      }
      delta_x = state.t_step * dot(RK::b, state.K_curr, RK::s);
      delta_x_hat = state.t_step * dot(RK::bb, state.K_curr, RK::s);

      if constexpr (RK::c[RK::s - 1] != 1.)
        state.t_curr = state.t_prev + state.t_step;
      state.x_curr = state.x_prev + delta_x;

      /*state.error_curr = norm(delta_x - delta_x_hat);*/
      state.error_curr = delta_x - delta_x_hat; // ?
    };

    events.start_events(state);
    events.step_events(state); // it is here so saving includes 0th step

    while (state.t_curr < final_time) {

      state.t_prev = state.t_curr;
      state.x_prev = state.x_curr;

      runge_kutta_step();

      // stepsize for the next step, even if this step is rejected
      /*state.t_step = stepsize_controller.template new_stepsize<RK>(state);*/
      bool reject_step = stepsize_controller.template set_stepsize<RK>(state);
      state.t_step = std::min(state.t_step, final_time - state.t_curr);

      if (reject_step) {
        events.reject_events(state);
        state.t_curr = state.t_prev;
        state.x_curr = state.x_prev;
        continue;
      }

      if (double t_event = events.locate(state);
          t_event < std::numeric_limits<double>::max()) {
        double save_t_step = state.t_step;

        state.t_step = t_event - state.t_prev;
        runge_kutta_step();
        events.located_event(state);

        // restore time step (important for constnat time step)
        state.t_step = save_t_step;
      }

      state.push_back_curr();

      events.step_events(state);
    }

    events.stop_events(state);

    return events.get_saved();
  }
};
} // namespace diffurch
