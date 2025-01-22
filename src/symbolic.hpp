#pragma once

#include "events.hpp"
#include <array>
#include <cstddef>
#include <filesystem>
#include <math.h>
#include <tuple>
#include <type_traits>
#include <utility>

namespace State {

struct StateExpression {};

template <typename T>
concept IsStateExpression = std::is_base_of_v<StateExpression, std::decay_t<T>>;

template <typename T>
concept IsNotStateExpression =
    !std::is_base_of_v<StateExpression, std::decay_t<T>>;

template <IsNotStateExpression T = double> struct Constant : StateExpression {
  T value;

  Constant(const T &val) : value(val) {}
  operator T() const { return value; }

  T operator()([[maybe_unused]] const auto &state) const { return value; }
  T prev([[maybe_unused]] const auto &state) const { return value; }
  T operator()([[maybe_unused]] const auto &state,
               [[maybe_unused]] double t) const {
    return value;
  }
  T operator()([[maybe_unused]] double t) const { return value; }
};

/*template <IsNotStateExpression T> struct Constant : StateExpression {*/
/*  template <typename U>*/
/*    requires(!std::is_rvalue_reference<U &&>::value)*/
/*  explicit Constant(U &&val) : value(std::forward<U>(val)) {}*/
/**/
/*  // For rvalue arguments: store by reference*/
/*  template <typename U>*/
/*    requires(std::is_rvalue_reference<U &&>::value)*/
/*  explicit Constant(U &&val) : value(val) {}*/
/**/
/*  std::conditional_t<std::is_rvalue_reference<T &&>::value, T &, T> value;*/
/**/
/*  T operator()(const auto &state) const { return value; }*/
/*  T prev(const auto &state) const { return value; }*/
/*  T operator()(const auto &state, double t) const { return value; }*/
/*  T operator()(double t) const { return value; }*/
/*};*/
/*template <typename T> Constant(T &&) -> Constant<T>;*/

struct TimeVariable : StateExpression {
  static auto operator()(const auto &state) { return state.t_curr; }
  static auto prev(const auto &state) { return state.t_prev; }
  static auto operator()([[maybe_unused]] const auto &state, double t) {
    return t;
  }
  static auto operator()(double t) { return t; }
  static auto get_events() { return Events(); }
};

template <size_t coordinate, IsStateExpression Arg, size_t derivative = 0>
struct VariableAt : StateExpression {
  Arg arg;
  VariableAt(Arg arg_) : arg(arg_) {}
  auto operator()(const auto &state) const {
    return state.template eval<derivative, coordinate>(arg(state));
  }
  auto prev(const auto &state) const {
    return state.template eval<derivative, coordinate>(arg.prev(state));
  }
  auto operator()(const auto &state, double t) const {
    return state.template eval<derivative, coordinate>(arg(state, t));
  }
  auto get_events() const { return arg.get_events(); }
};

template <size_t coordinate = -1> struct Variable : StateExpression {
  static constexpr auto operator()(const IsNotStateExpression auto &state) {
    if constexpr (coordinate == -1)
      return state.x_curr;
    else
      return state.x_curr[coordinate];
  }
  static auto prev(const IsNotStateExpression auto &state) {
    if constexpr (coordinate == -1)
      return state.x_prev;
    else
      return state.x_prev[coordinate];
  }
  static auto operator()(const IsNotStateExpression auto &state, double t) {
    return state.template eval<0, coordinate>(t);
  }

  template <IsStateExpression Arg> static auto operator()(Arg arg) {
    return VariableAt<coordinate, Arg, 0>(arg);
  }

  static auto get_events() { return Events(); }
};

template <size_t derivative = 1, size_t coordinate = -1>
constexpr auto D(const Variable<coordinate> &var) {
  return VariableAt<coordinate, TimeVariable, derivative>(TimeVariable());
}

template <size_t N, size_t from = 0, size_t to = N> constexpr auto Variables() {
  if constexpr (from == to) {
    return std::tuple<>();
  } else {
    return std::tuple_cat(std::make_tuple(Variable<from>()),
                          Variables<N, from + 1, to>());
  }
}

template <typename... Coordinates> struct Vector : StateExpression {
  std::tuple<Coordinates...> coordinates;

  Vector(Coordinates... coordinates_)
      : coordinates(std::make_tuple(coordinates_...)) {};
  Vector(std::tuple<Coordinates...> coordinates_)
      : coordinates(coordinates_) {};

  auto operator()(const auto &state) const {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(state)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto operator()(const auto &state, double t) const {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(state, t)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto operator()(double t) const {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(t)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto prev(const auto &state) const {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates).prev(state)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto get_events() const {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return Events(std::get<Is>(coordinates).get_events()...);
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
};

#define STATE_OPERATOR_OVERLOAD(op, op_name)                                   \
  template <IsStateExpression L, IsStateExpression R>                          \
  struct op_name : StateExpression {                                           \
    L l;                                                                       \
    R r;                                                                       \
                                                                               \
    op_name(L l_, R r_) : l(l_), r(r_) {};                                     \
                                                                               \
    auto operator()(const auto &state) const { return l(state) op r(state); }  \
    auto prev(const auto &state) const {                                       \
      return l.prev(state) op r.prev(state);                                   \
    }                                                                          \
    auto operator()(const auto &state, double t) const {                       \
      return l(state, t) op r(state, t);                                       \
    }                                                                          \
    auto operator()(double t) const { return l(t) op r(t); }                   \
    auto get_events() const { return Events(l.get_events(), r.get_events()); } \
  };                                                                           \
  template <IsStateExpression L, IsStateExpression R>                          \
  auto operator op(L l, R r) {                                                 \
    return op_name(l, r);                                                      \
  }                                                                            \
  template <IsNotStateExpression L, IsStateExpression R>                       \
  auto operator op(L &&l, R r) {                                               \
    return op_name(Constant(std::forward<L>(l)), r);                           \
  }                                                                            \
  template <IsStateExpression L, IsNotStateExpression R>                       \
  auto operator op(L l, R &&r) {                                               \
    return op_name(l, Constant(std::forward<R>(r)));                           \
  }

STATE_OPERATOR_OVERLOAD(+, Add);
STATE_OPERATOR_OVERLOAD(-, Sub);
STATE_OPERATOR_OVERLOAD(*, Mul);
STATE_OPERATOR_OVERLOAD(/, Div);

#define STATE_UNARY_OPERATOR_OVERLOAD(op, op_name)                             \
  template <IsStateExpression Arg> struct op_name : StateExpression {          \
    Arg arg;                                                                   \
    op_name(Arg arg_) : arg(arg_) {}                                           \
    auto operator()(const auto &state) const { return op(arg(state)); }        \
    auto operator()(const auto &state, double t) const {                       \
      return op(arg(state, t));                                                \
    }                                                                          \
    auto operator()(double t) const { return op(arg(t)); }                     \
    auto prev(const auto &state) const { return op(arg.prev(state)); }         \
    auto get_events() const { return arg.get_events(); }                       \
  };                                                                           \
  template <IsStateExpression Arg> auto operator op(Arg arg) {                 \
    return op_name(arg);                                                       \
  }

STATE_UNARY_OPERATOR_OVERLOAD(-, Neg);
STATE_UNARY_OPERATOR_OVERLOAD(+, UnaryPlus);

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

template <IsStateExpression Arg> struct Function_sign : StateExpression {
  Arg arg;
  Function_sign(Arg arg_) : arg(arg_) {}
  double value;
  auto operator()(const auto &state) const { return value; }
  auto operator()(const auto &state, double t) const {
    return sign(arg(state, t));
  }
  auto operator()(double t) const { return sign(arg(t)); }
  auto prev(const auto &state) const { return value; }

  auto set_value(const auto &state) { value = sign(arg(state)); }

  auto get_events() const {
    auto set_value_lambda = [this](const auto &state) { set_value(state); };
    return Events(arg.get_events(), StartEvent(nullptr, set_value_lambda),
                  Event(arg == 0, nullptr, set_value_lambda));
  }
};
template <IsStateExpression Arg> auto sign(Arg arg) {
  return Function_sign(arg);
}

/*template <IsStateExpression Arg, IsStateExpression Case1, IsStateExpression
 * Case2>*/
/*struct Function_piecewise {*/
/*  Arg arg;*/
/*  Case1 case1;*/
/*  Case2 case2;*/
/*  bool flag;*/
/**/
/*  Function_piecewise(Arg arg_, Case1 case1_, Case2 case2_)*/
/*      : arg(arg_), case1(case1_), case2(case2_) {};*/
/**/
/*  auto operator()(const auto &state) const {*/
/*    if (flag)*/
/*      return case2(state);*/
/*    else*/
/*      return case1(state);*/
/*  }*/
/*};*/

template <typename Derived> struct EventFunctionInterface {
  double locate(const auto &state) const {
    auto self = static_cast<Derived *>(this);
    if (self->detect(state)) {
      return find_root([&state, &self](double t) { return (*self)(state, t); },
                       state.t_prev, state.t_curr);
    } else {
      return std::numeric_limits<double>::max();
    }
  }
};

template <IsStateExpression L, IsStateExpression R>
struct EqualEventFunction
/*: EventFunctionInterface<EqualEventFunction> */
{
  L l;
  R r;

  EqualEventFunction(L l_, R r_) : l(l_), r(r_) {};

  bool detect(const auto &state) const {
    auto curr = l(state) - r(state);
    auto prev = l.prev(state) - r.prev(state);
    return (curr * prev < 0) || curr == 0;
    // benchmark against
    /*auto l_curr = l(state);*/
    /*auto r_curr = r(state);*/
    /*auto l_prev = l.prev(state);*/
    /*auto r_prev = r.prev(state);*/
    /*return (l_curr >= r_curr && l_prev < r_prev) ||*/
    /*       (l_curr <= r_curr && l_prev > r_prev);*/
  }
};

template <IsStateExpression L, IsStateExpression R>
struct GreaterEventFunction {
  L l;
  R r;

  GreaterEventFunction(L l_, R r_) : l(l_), r(r_) {};

  bool detect(const auto &state) const {
    return l(state) >= r(state) && l.prev(state) < r.prev(state);
  }
};

/*
 Events:
 x == 0
 Dx <= 0


 Conditions:
 x > 0
 Dx < 0

 Combined:
 x == 0 && x[t-1] < 0 && y > 0
 */

/*template <IsStateExpression L, IsStateExpression R> auto operator op(L l, R r)
 * {        */
/*  return op_name(l, r); */
/*} */
/*template <typename L, IsStateExpression R> auto operator op(L l, R r) { */
/*  return op_name(Constant(l), r); */
/*} */
/*template <IsStateExpression L, typename R> auto operator op(L l, R r) { */
/*  return op_name(l, Constant(r)); */
/*}*/

} // namespace State
