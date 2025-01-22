#pragma once

#include "expression.hpp"

#include "../util/find_root.hpp"
#include <limits>

namespace State {

template <typename Arg> struct EqualEvent : StateEventExpression {
  Arg arg;
  EqualEvent(Arg arg_) : arg(arg_) {};

  bool detect(const auto &state) const {
    auto curr = arg(state);
    auto prev = arg.prev(state);
    return (curr * prev < 0) || curr == 0;
    // benchmark against
    /*auto l_curr = l(state);*/
    /*auto r_curr = r(state);*/
    /*auto l_prev = l.prev(state);*/
    /*auto r_prev = r.prev(state);*/
    /*return (l_curr >= r_curr && l_prev < r_prev) ||*/
    /*       (l_curr <= r_curr && l_prev > r_prev);*/
  }

  double locate(const auto &state) const {
    if (detect(state)) {
      return root_by_bisection(
          [this, &state](double t) { return arg(state, t); }, state.t_prev,
          state.t_curr);
    } else {
      return std::numeric_limits<double>::max();
    }
  }
};

template <typename Arg> struct EqualToPosEvent : EqualEvent<Arg> {

  using EqualEvent<Arg>::arg;
  using EqualEvent<Arg>::EqualEvent;
  using EqualEvent<Arg>::locate;

  bool detect(const auto &state) const {
    return arg(state) >= 0 && arg.prev(state) < 0;
  }
};

template <typename Arg> struct EqualToNegEvent : EqualEvent<Arg> {

  using EqualEvent<Arg>::arg;
  using EqualEvent<Arg>::EqualEvent;
  using EqualEvent<Arg>::locate;

  bool detect(const auto &state) const {
    return arg(state) <= 0 && arg.prev(state) > 0;
  }
};

#define EVENT_OVERLOAD_OPERATOR(op, op_name)                                   \
  template <IsStateExpression L, IsStateExpression R>                          \
  auto operator op(L l, R r) {                                                 \
    return op_name(l - r);                                                     \
  }                                                                            \
  template <IsNotStateExpression L, IsStateExpression R>                       \
  auto operator op(L l, R r) {                                                 \
    return op_name(Constant(l) - r);                                           \
  }                                                                            \
  template <IsStateExpression L, IsNotStateExpression R>                       \
  auto operator op(L l, R r) {                                                 \
    return op_name(l - Constant(r));                                           \
  }

EVENT_OVERLOAD_OPERATOR(==, EqualEvent);
EVENT_OVERLOAD_OPERATOR(!=, EqualToNegEvent);
EVENT_OVERLOAD_OPERATOR(<=>, EqualToPosEvent);

template <IsStateEventExpression Event, IsStateBoolExpression Condition>
struct StateEventWithLocationCondition : StateEventExpression {
  Event event;
  Condition condition;
  StateEventWithLocationCondition(Event event_, Condition condition_)
      : event(event_), condition(condition_) {};

  bool detect(const auto &state) const { return event.detect(state); }

  bool locate(const auto &state) const {
    if (!detect(state))
      return std::numeric_limits<double>::max();

    double t =
        root_by_bisection([this, &state](double t) { return arg(state, t); },
                          state.t_prev, state.t_curr);
    if (!condition(state, t))
      return std::numeric_limits<double>::max();

    else
      return t;
  }
};

template <IsStateEventExpression Event, IsStateBoolExpression Condition>
struct StateEventWithDetectionCondition : StateEventExpression {
  Event event;
  Condition condition;
  StateEventWithDetectionCondition(Event event_, Condition condition_)
      : event(event_), condition(condition_) {};

  bool detect(const auto &state) const {
    return event.detect(state) && (condition(state) || condition.prev(state));
  }

  bool locate(const auto &state) const {
    if (!detect(state))
      return std::numeric_limits<double>::max();

    return root_by_bisection([this, &state](double t) { return arg(state, t); },
                             state.t_prev, state.t_curr);
  }
};

#define EVENT_WITH_CONDITION_OVERLOAD_OPERATOR(op, op_name)                    \
  template <IsStateEventExpression Event, IsStateBoolExpression Condition>     \
  auto operator op(Event event, Condition condition) {                         \
    return op_name(event, condition);                                          \
  }

// has precedence 14
EVENT_WITH_CONDITION_OVERLOAD_OPERATOR(&&, StateEventWithDetectionCondition);
// has precedence 11
EVENT_WITH_CONDITION_OVERLOAD_OPERATOR(&, StateEventWithLocationCondition);

} // namespace State
