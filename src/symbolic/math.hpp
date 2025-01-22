#pragma once

#include "../events.hpp"
#include "expression.hpp"
#include <math.h>
#include <utility>

namespace State {

#define STATE_FUNCTION_OVERLOAD(func)                                          \
  template <IsStateExpression Arg> struct Function_##func : StateExpression {  \
    Arg arg;                                                                   \
    Function_##func(Arg arg_) : arg(arg_) {}                                   \
    auto operator()(const auto &state) const { return func(arg(state)); }      \
    auto operator()(const auto &state, double t) const {                       \
      return func(arg(state, t));                                              \
    }                                                                          \
    auto operator()(double t) const { return func(arg(t)); }                   \
    auto prev(const auto &state) const { return func(arg.prev(state)); }       \
    auto get_events() const { return arg.get_events(); }                       \
  };                                                                           \
  template <IsStateExpression Arg> auto func(Arg arg) {                        \
    return Function_##func(arg);                                               \
  }

STATE_FUNCTION_OVERLOAD(sin);
STATE_FUNCTION_OVERLOAD(cos);
STATE_FUNCTION_OVERLOAD(tan);
STATE_FUNCTION_OVERLOAD(cot);
STATE_FUNCTION_OVERLOAD(exp);
STATE_FUNCTION_OVERLOAD(log);
STATE_FUNCTION_OVERLOAD(log10);
STATE_FUNCTION_OVERLOAD(log2);

/*VARIABLE_OVERLOAD_FUNCTION_2(pow);*/
/*VARIABLE_OVERLOAD_FUNCTION_2(atan2);*/

} // namespace State
