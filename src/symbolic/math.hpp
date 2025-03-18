#pragma once

#include "../util/math.hpp"
#include "symbol_types.hpp"
#include <math.h>
#include <tuple>

namespace diffurch {

#define STATE_FUNCTION_OVERLOAD(func, func_derivative)                         \
  template <IsSymbol Arg> struct Function_##func : Symbol {                    \
    Arg arg;                                                                   \
    Function_##func(Arg arg_) : arg(arg_) {}                                   \
    auto operator()(const auto &state) const { return func(arg(state)); }      \
    auto operator()(const auto &state, double t) const {                       \
      return func(arg(state, t));                                              \
    }                                                                          \
    auto operator()(double t) const { return func(arg(t)); }                   \
    auto prev(const auto &state) const { return func(arg.prev(state)); }       \
    template <size_t current_coordinate = size_t(-1)> auto get_events() {      \
      return arg.template get_events<current_coordinate>();                    \
    }                                                                          \
  };                                                                           \
  template <IsSymbol Arg> auto func(Arg arg) { return Function_##func(arg); }  \
  template <size_t derivative = 1, IsSymbol Arg>                               \
  constexpr auto D(const Function_##func<Arg> &func) {                         \
    if constexpr (derivative == 0)                                             \
      return func;                                                             \
    else                                                                       \
      return D<derivative - 1>(func_derivative(func.arg)) * D(func.arg);       \
  }

#define STATE_FUNCTION_OVERLOAD_2(func)                                        \
  template <IsSymbol Arg1, IsSymbol Arg2> struct Function_##func : Symbol {    \
    Arg1 arg1;                                                                 \
    Arg2 arg2;                                                                 \
    Function_##func(Arg1 arg1_, Arg2 arg2_) : arg1(arg1_), arg2(arg2_) {}      \
    auto operator()(const auto &state) const {                                 \
      return func(arg1(state), arg2(state));                                   \
    }                                                                          \
    auto operator()(const auto &state, double t) const {                       \
      return func(arg1(state, t), arg2(state, t));                             \
    }                                                                          \
    auto operator()(double t) const { return func(arg1(t), arg2(t)); }         \
    auto prev(const auto &state) const {                                       \
      return func(arg1.prev(state), arg2.prev(state));                         \
    }                                                                          \
    template <size_t current_coordinate = size_t(-1)> auto get_events() {      \
      return std::tuple_cat(arg1.template get_events<current_coordinate>(),    \
                            arg2.template get_events<current_coordinate>());   \
    }                                                                          \
  };                                                                           \
  template <IsSymbol Arg1, IsSymbol Arg2> auto func(Arg1 arg1, Arg2 arg2) {    \
    return Function_##func(arg1, arg2);                                        \
  }                                                                            \
  template <IsNotSymbol Arg1, IsSymbol Arg2> auto func(Arg1 arg1, Arg2 arg2) { \
    return Function_##func(Constant(arg1), arg2);                              \
  }                                                                            \
  template <IsSymbol Arg1, IsNotSymbol Arg2> auto func(Arg1 arg1, Arg2 arg2) { \
    return Function_##func(arg1, Constant(arg2));                              \
  }

STATE_FUNCTION_OVERLOAD(sin, cos);
STATE_FUNCTION_OVERLOAD(cos, -sin);
STATE_FUNCTION_OVERLOAD(tan, [](const auto &x) { return pow(cos(x), -2); });
/*STATE_FUNCTION_OVERLOAD(cot);*/
STATE_FUNCTION_OVERLOAD(exp, exp);
STATE_FUNCTION_OVERLOAD(log, [](const auto &x) { return 1 / x; });
STATE_FUNCTION_OVERLOAD(log10,
                        [](const auto &x) { return 1 / (x * std::log(10.)); });
STATE_FUNCTION_OVERLOAD(log2,
                        [](const auto &x) { return 1 / (x * std::log(2.)); });

STATE_FUNCTION_OVERLOAD(sign, [](auto...) { return 0.; });
STATE_FUNCTION_OVERLOAD(abs, sign);
STATE_FUNCTION_OVERLOAD(relu, step);
STATE_FUNCTION_OVERLOAD(step, [](auto...) { return 0.; });

using ::std::pow;
STATE_FUNCTION_OVERLOAD_2(pow);
STATE_FUNCTION_OVERLOAD_2(atan2);

template <IsSymbol Arg, typename Callable> struct state_function : Symbol {

  Arg arg;
  Callable func;

  state_function(Arg arg_, Callable func_) : arg(arg_), func(func_) {}

  auto operator()(const auto &state) const { return func(arg(state)); }
  auto operator()(const auto &state, double t) const {
    return func(arg(state, t));
  }
  auto operator()(double t) const { return func(arg(t)); }
  auto prev(const auto &state) const { return func(arg.prev(state)); }
  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return arg.template get_events<current_coordinate>();
  }
};

} // namespace diffurch
