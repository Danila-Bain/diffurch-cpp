#pragma once

#include "expression.hpp"
#include "operators.hpp"

#include "../util/find_root.hpp"
#include <limits>

namespace diffurch {

template <IsStateBoolExpression Arg> struct WhenSwitch : StateDetectExpression {
  Arg arg;
  WhenSwitch(Arg arg_) : arg(arg_) {};

  bool detect(const auto &state) const { return arg(state) != arg.prev(state); }

  double locate(const auto &state) const {
    if (detect(state)) {
      return bool_change_by_bisection(
          [this, &state](double t) { return arg(state, t); }, state.t_prev,
          state.t_curr);
    } else {
      return std::numeric_limits<double>::max();
    }
  }
};

template <IsStateExpression Arg> struct WhenZeroCross : StateDetectExpression {
  Arg arg;
  WhenZeroCross(Arg arg_) : arg(arg_) {};

  bool detect(const auto &state) const {
    auto curr = arg(state);
    auto prev = arg.prev(state);
    // std::cout << "detecting zero cross between " << prev << " and " << curr
    // << std::endl;
    return (curr * prev < 0) || curr == 0;
    // benchmark against (the following doesn't have additional implicit
    // substractions)
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

template <IsStateExpression Arg>
struct ZeroCrossFromBelowEvent : WhenZeroCross<Arg> {
  using WhenZeroCross<Arg>::arg;
  using WhenZeroCross<Arg>::EqualEvent;
  using WhenZeroCross<Arg>::locate;
  bool detect(const auto &state) const {
    return arg(state) >= 0 && arg.prev(state) < 0;
  }
};
template <IsStateExpression Arg>
struct WhenZeroCrossFromAboveEvent : WhenZeroCross<Arg> {
  using WhenZeroCross<Arg>::arg;
  using WhenZeroCross<Arg>::EqualEvent;
  using WhenZeroCross<Arg>::locate;
  bool detect(const auto &state) const {
    return arg(state) <= 0 && arg.prev(state) > 0;
  }
};

// support for When(x == 0) or When(x > 0) syntax
#define STATE_CROSS_EVENT_FROM_COMP(comparison_operator, cross_event_type)     \
  template <IsStateExpression L, IsStateExpression R>                          \
  auto When(const comparison_operator<L, R> &bool_expr) {                      \
    return cross_event_type(bool_expr.l - bool_expr.r);                        \
  }

STATE_CROSS_EVENT_FROM_COMP(Equal, WhenZeroCross);
STATE_CROSS_EVENT_FROM_COMP(Greater, WhenZeroCrossFromBelow);
STATE_CROSS_EVENT_FROM_COMP(GreaterEqual, WhenZeroCrossFromBelow);
STATE_CROSS_EVENT_FROM_COMP(Less, WhenZeroCrossFromAbove);
STATE_CROSS_EVENT_FROM_COMP(LessEqual, WhenZeroCrossFromAbove);

template <IsStateDetectExpression Event, IsStateBoolExpression Condition>
struct StateDetectWithLocationCondition : StateDetectExpression {
  Event event;
  Condition condition;
  StateDetectWithLocationCondition(Event event_, Condition condition_)
      : event(event_), condition(condition_) {};

  bool detect(const auto &state) const { return event.detect(state); }

  bool locate(const auto &state) const {
    if (!detect(state))
      return std::numeric_limits<double>::max();

    double t =
        root_by_bisection([this, &state](double t_) { return arg(state, t_); },
                          state.t_prev, state.t_curr);
    if (!condition(state, t))
      return std::numeric_limits<double>::max();

    else
      return t;
  }
};

template <IsStateDetectExpression Event, IsStateBoolExpression Condition>
struct StateDetectWithDetectionCondition : StateDetectExpression {
  Event event;
  Condition condition;
  StateDetectWithDetectionCondition(Event event_, Condition condition_)
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
  template <IsStateDetectExpression Event, IsStateBoolExpression Condition>    \
  auto operator op(Event event, Condition condition) {                         \
    return op_name(event, condition);                                          \
  }

// has precedence 14
EVENT_WITH_CONDITION_OVERLOAD_OPERATOR(&&, StateDetectWithDetectionCondition);
// has precedence 11
EVENT_WITH_CONDITION_OVERLOAD_OPERATOR(&, StateDetectWithLocationCondition);

} // namespace diffurch
