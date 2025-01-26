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

#include "rk_tables/rk98.hpp"

// This class is intended to be exclusively used
// with Curiously Recurring Template Pattern (CRTP),
// that is in creating other classes like
// ``struct Lorenz : Solver<Lorenz> {...};``,
// that inherit solver with themselves.
template <typename Equation> struct Solver {

  auto get_events() { return Events(); }

  template <typename RK = rk98, typename StepsizeControllerT = ConstantStepsize>
  auto
  solution(double initial_time, double final_time,
           StepsizeControllerT stepsize_controller = ConstantStepsize(0.1)) {

    return solution<RK>(initial_time, final_time, stepsize_controller,
                        Events(StepEvent(SaveAll<Equation>())));
  }

  template <typename RK = rk98, typename StepsizeControllerT,
            typename AdditionalEventsT>
  auto solution(double initial_time, double final_time,
                StepsizeControllerT stepsize_controller,
                AdditionalEventsT additional_events) {
    auto self = static_cast<Equation *>(this);
    auto lhs = self->get_lhs();
    auto ic = self->get_ic();
    static constexpr size_t n = std::tuple_size<decltype(ic(0.))>::value;

    auto events =
        Events(Events(self->get_events(), lhs.get_events()), additional_events);

    double stepsize = stepsize_controller.initial_stepsize();

    auto state = State::State<RK, decltype(ic)>(initial_time, ic);

    Vec<n> delta_x;
    Vec<n> delta_x_hat;
    double error;

    auto runge_kutta_step = [&, this]() {
      for (size_t i = 0; i < RK::s; i++) {
        state.t_curr = state.t_prev + stepsize * RK::c[i];
        state.x_curr = state.x_prev + stepsize * dot(RK::a[i], state.K_curr, i);
        state.K_curr[i] = lhs(state);
        events.call_events(state);
      }
      delta_x = stepsize * dot(RK::b, state.K_curr, RK::s);
      delta_x_hat = stepsize * dot(RK::bb, state.K_curr, RK::s);
      error = norm(delta_x - delta_x_hat);

      if constexpr (RK::c[RK::s - 1] != 1.)
        state.t_curr = state.t_prev + stepsize;
      state.x_curr = state.x_prev + delta_x;
    };

    events.start_events(state);
    events.step_events(state);

    while (state.t_curr < final_time) {

      state.t_prev = state.t_curr;
      state.x_prev = state.x_curr;

      // Runge-Kutta step
      runge_kutta_step();
      // stepsize for the next step, even if this step is rejected
      stepsize = min(final_time - state.t_curr, // unsafe??
                     stepsize_controller.update_stepsize(stepsize, error));

      if (stepsize_controller.reject_step(error)) {
        events.reject_events(state);
        continue;
      }

      if (double t_event = events.locate(state);
          t_event < std::numeric_limits<double>::max()) {
        stepsize = t_event - state.t_prev;
        runge_kutta_step();
        events.located_event(state);
      }

      state.push_back_curr();

      events.step_events(state);
    }

    events.stop_events(state);

    return events.get_saved();
  }
};
