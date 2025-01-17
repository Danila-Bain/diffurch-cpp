#include "equation.hpp"
#include "events.hpp"
#include "state.hpp"
#include "state_symbols.hpp"
#include "vec.hpp"
#include <algorithm>
#include <boost/preprocessor.hpp>
#include <cstddef>
#include <tuple>
#include <vector>

#include "rk_tables/rk98.hpp"
using RK = rk98;

// This class is intended to be exclusively used
// with Curiously Recurring Template Pattern (CRTP),
// that is in creating other classes like
// ``struct Lorenz : Solver<Lorenz> {...};``,
// that inherit solver with themselves.
template <typename Equation> struct Solver {
  auto solution(double initial_time, double final_time) {
    auto self = static_cast<Equation *>(this);
    constexpr size_t n = order_of_equation<Equation>::value;

    auto lhs = self->get_lhs();
    auto ic = self->get_ic();

    auto [detection_events, step_events, reject_events, call_events,
          start_events, stop_events] =
        Events(self->get_events(), lhs.get_events());

    double stepsize = stepsize_controller.initial_stepsize();

    auto state = State::State<Equation, RK>(initial_time, self);

    array<Vec<n>, RK::s> K;
    Vec<n> delta_x;
    Vec<n> delta_x_hat;
    double error;

    auto rk_step = [&]() {
      for (int i = 0; i < RK::s; i++) {
        state.curr_t = state.prev_t + stepsize * RK::C[i];
        state.curr_x = state.prev_x + stepsize * dot(RK::A[i], K, i);
        K[i] = self->lhs(state);
      }
      delta_x = stepsize * dot(RK::B, K, RK::s);
      delta_x_hat = stepsize * dot(RK::B_hat, K, RK::s);
      error = norm(delta_x - delta_x_hat);
    };

    // before integration events

    while (state.curr_t < final_time) {

      // Runge-Kutta step
      rk_step();

      // stepsize for the next step, even if this step is rejected
      stepsize = stepsize_controller.update_stepsize(stepsize, error);

      if (stepsize_controller.reject_step(error)) {
        /*step_rejected_events;*/
        continue;
      }

      state.curr_t = state.prev_t + stepsize;
      state.curr_x = state.prev_x + delta_x;

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

      state.prev_t = state.curr_t;
      state.prev_x = state.curr_x;
    }

    // stop_integration_events

    // returns
  }
};

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

auto t = State::TimeVariable();
auto [x, y, z] = State::Variables<3>();

struct Lorenz : Solver<Lorenz> {

  double sigma, rho, beta;

  Lorenz(double sigma_, double rho_, double beta_)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  auto get_lhs() {
    return State::Vector(sigma * (y - x), rho * x - x * z, -beta * z + x * z);
  }

  auto get_ic() { return State::Vector(sin(t), cos(t), t * t); }

  auto get_events() { return Events(StepEvent(t)); }
  /**/
  /*Events events(WhenEqualZero(x - y) | WhileGreaterZero(x + y) | Save(t) |*/
  /*                  Set(counter++),*/
  /*              WhenStep() | Save(t, x, y, z),*/
  /*              WhenGreater(abs(x), 1000) | StopIntegration(),*/
  /*              WhenStopIntegration() | Save(make_tuple(x, y, z)),*/
  /*              WhenStepRejected() | Save(t) |*/
  /*                  Set([&]() { rejected_counter++ }),*/
  /*              WhenZero(z[t - 1]));*/
};

struct HarmonicOscillator : Solver<HarmonicOscillator>, RK98 {
  TimeVariable t;
  inline static auto [x, Dx] = Statevariables<2>();
  double x_0, Dx_0;

  LHS lhs(Dx, -x);
  IC initial_condition(x_0 *cos(t) + Dx_0 * sin(t),
                       -x_0 *sin(t) + Dx_0 * cos(t));

  // sensible default;
  Events events(WhenStep() | SaveState());
  /*Events events(SaveSteps());*/
};

struct RelayOscillator : Solver<RelayOscillator>, RK98 {
  State::Time t;
  State::Variable<0> x;
  State::Variable<1> Dx;

  State::Vector lhs(Dx, -sign(x));
  State::Vector initial_condition(1, 0);

  Events events(StepEvent(make_tuple(t, x, Dx)));
};
