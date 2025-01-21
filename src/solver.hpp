#include "events.hpp"
#include "state.hpp"
#include "stepsize.hpp"
#include "symbolic.hpp"
#include "util/vec.hpp"
#include <boost/preprocessor.hpp>
#include <cstddef>
#include <limits>

#include "rk_tables/rk98.hpp"
using RK = rk98;

using StepsizeController = ConstantStepsize;

// This class is intended to be exclusively used
// with Curiously Recurring Template Pattern (CRTP),
// that is in creating other classes like
// ``struct Lorenz : Solver<Lorenz> {...};``,
// that inherit solver with themselves.
template <typename Equation> struct Solver {
  auto solution(double initial_time, double final_time) {
    auto self = static_cast<Equation *>(this);
    auto lhs = self->get_lhs();
    auto ic = self->get_ic();
    static constexpr size_t n = std::tuple_size<decltype(ic(0.))>::value;

    auto events = Events(self->get_events(), lhs.get_events());

    auto stepsize_controller = StepsizeController(0.05);

    double stepsize = stepsize_controller.initial_stepsize();

    auto state = State::State<RK, decltype(ic)>(initial_time, ic);

    Vec<n> delta_x;
    Vec<n> delta_x_hat;
    double error;

    events.start_events(state);

    while (state.t_curr < final_time) {

      state.t_prev = state.t_curr;
      state.x_prev = state.x_curr;

      // Runge-Kutta step
      {
        for (size_t i = 0; i < RK::s; i++) {
          state.t_curr = state.t_prev + stepsize * RK::c[i];
          state.x_curr =
              state.x_prev + stepsize * dot(RK::a[i], state.K_curr, i);
          state.K_curr[i] = lhs(state);
          events.call_events(state);
        }
        delta_x = stepsize * dot(RK::b, state.K_curr, RK::s);
        delta_x_hat = stepsize * dot(RK::bb, state.K_curr, RK::s);
        error = norm(delta_x - delta_x_hat);

        if constexpr (RK::c[RK::s - 1] != 1.)
          state.t_curr = state.t_prev + stepsize;
        state.x_curr = state.x_prev + delta_x;
      }
      /*std::cout << state.x_curr << std::endl;*/

      // stepsize for the next step, even if this step is rejected
      if (events.located_event_index != size_t(-1)) {
        events.located_event(state);
      } else if (double t_event = events.locate(state);
                 t_event < std::numeric_limits<double>::max()) {
        stepsize = t_event - state.t_prev;
        continue;
      }

      stepsize = stepsize_controller.update_stepsize(stepsize, error);

      if (stepsize_controller.reject_step(error)) {
        events.reject_events(state);
        continue;
      }

      state.push_back_curr();

      events.step_events(state);
    }

    events.stop_events(state);

    return events.get_saved();
  }
};

// step_events
//
/*
Event handling:
    When events:
        * Event detection/location is called for each event
            * If multiple events are detected on one step, only the
              earliest is processed
            * Detection is done with just state function, like
              if (state_function(state) * state_function.prev(state) < 0)
            * Location is done using some root finding method,
              applied to interpolated state:
              t -> state_function(state, t)
        * Step is redone to step-on located event
        * Step callback is called:
            * Save current state to event's container
            * Change lhs function switches
            * If some state-chaning functions are there, they are forced
              to create new zero-sized step with changes applied,
              and call save for both of them
    Step events:
        * After each successful step, i.e., after step is accepted by
stepsize controller, after when-event is processed
 */

// when events
/*if (when_event.disable)*/
/*  when_event.disable = false;*/
/*else if (when_event.detect(state)) {*/
/*  double t_event = step_event.locate(state);*/
/*  stepsize = t_event - state.prev_t;*/
/*  when_event.disable = true;*/
/*  continue;*/
/*}*/

/*struct Lorens : Solver<Lorenz, RK98> {*/
/*  double sigma;*/
/*  double rho;*/
/*  double beta;*/
/**/
/*  auto lhs(const auto &state) const {*/
/*    const auto &[x, y, z] = state.curr_x;*/
/*    return Vec<3>{sigma * (y - x), rho * x - x * z, -beta * z + x * z};*/
/*  }*/
/**/
/*  auto initial_conditions(double t) const { return Vec<3>{1, 2, beta}; }*/
/**/
/*  // constructor -> ???*/
/**/
/*  auto events() {*/
/*    using namespace Events;*/
/*    return Events(Event(WhenStep{}, Save(__state_lambda__(x, y, z)(*/
/*                                        make_tuple(t, x, y, z)))),*/
/*                  Event(When([this] __state_lambda__(x, y, z)(x + y - 1)),*/
/*                        Save([this] __state_lambda__(x, y, z)(t))))*/
/*  }*/
/**/
/*  auto events() {*/
/*    return Events {*/
/*         StepEvent(Save([this]__state_lambda__(x,y,z)(make_tuple(t,x,y,z)))),*/
/*         WhenEvent([this]__state_lambda_(x,y,z)(x_<1>(t) - y_<1>(t) + x - y),
 * Change(__state_lambda__(x,y,z)(x = -x))*/
/*    }*/
/*  }*/
/**/
/*  auto events() {*/
/*    Symbol t, x, y, z;*/
/*    return Events {*/
/*      WhenZero(x - y) | Save(t, x, y, z), WhenStep | Set(step_count++;)*/
/*    };*/
/*  }*/
/*};*/

/*struct RelayOscillator : Solver<RelayOscillator>, RK98 {*/
/*  State::Time t;*/
/*  State::Variable<0> x;*/
/*  State::Variable<1> Dx;*/
/**/
/*  State::Vector lhs(Dx, -sign(x));*/
/*  State::Vector initial_condition(1, 0);*/
/**/
/*  Events events(StepEvent(make_tuple(t, x, Dx)));*/
/*};*/
