#include "events.hpp"
/*#include "state.hpp"*/
#include <array>
#include <cstddef>
#include <math.h>
#include <tuple>
#include <type_traits>
#include <vector>
#include <array>

namespace State {

struct StateExpression {};

template <typename T>
concept IsStateExpression = std::is_base_of_v<StateExpression, T>;

template <typename T>
  requires(!IsStateExpression<T>)
struct Constant : StateExpression {
  Constant(T value_) : value(value_) {};
  T value;

  T operator()(const auto &state) const { return value; }
  T prev(const auto &state) const { return value; }
  T operator()(const auto &state, double t) const { return value; }
  T operator()(double t) const { return value; }
};

struct TimeVariable : StateExpression {
  static auto operator()(const auto &state) { return state.curr_t; }
  static auto prev(const auto &state) { return state.prev_t; }
  static auto operator()(const auto &state, double t) { return t; }
  static auto operator()(double t) { return t; }
  auto get_events() { return Events(); }
};

template <size_t coordinate, IsStateExpression Arg>
struct VariableAt : StateExpression {
  Arg arg;
  VariableAt(Arg arg_) : arg(arg_) {}
  auto operator()(const auto &state) const {
    return state.template eval<0, coordinate>(arg(state));
  }
  auto prev(const auto &state) const {
    return state.template eval<0, coordinate>(arg.prev(state));
  }
  auto operator()(const auto &state, double t) const {
    return state.template eval<0, coordinate>(arg(state, t));
  }
  auto get_events() { return arg.get_events(); }
};

template <size_t coordinate = -1> struct Variable : StateExpression {
  static constexpr auto operator()(const auto &state) {
    if constexpr (coordinate == -1)
      return state.curr_x;
    else
      return state.curr_x[coordinate];
  }
  static auto prev(const auto &state) {
    if constexpr (coordinate == -1)
      return state.prev_x;
    else
      return state.prev_x[coordinate];
  }
  static auto operator()(const auto &state, double t) {
    return state.template eval<0, coordinate>(t);
  }

  template <IsStateExpression Arg> auto operator[](Arg arg) {
    return VariableAt<coordinate, Arg>(arg);
  }

  auto get_events() { return Events(); }
};

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
    auto get_events() { return Events(l.get_events(), r.get_events()); }       \
  };                                                                           \
  template <IsStateExpression L, IsStateExpression R>                          \
  auto operator op(L l, R r) {                                                 \
    return op_name(l, r);                                                      \
  }                                                                            \
  template <typename L, IsStateExpression R> auto operator op(L l, R r) {      \
    return op_name(Constant(l), r);                                            \
  }                                                                            \
  template <IsStateExpression L, typename R> auto operator op(L l, R r) {      \
    return op_name(l, Constant(r));                                            \
  }

STATE_OPERATOR_OVERLOAD(+, Add);
STATE_OPERATOR_OVERLOAD(-, Sub);
STATE_OPERATOR_OVERLOAD(*, Mul);
STATE_OPERATOR_OVERLOAD(/, Div);

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
    auto get_events() { return Events(arg.get_events(), arg.get_events()); }   \
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

  auto get_events() {
    return Events(arg.get_events(),
                  Event(arg == 0, nullptr,
                        [this](const auto &state) { set_value(state); }));
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
                       state.prev_t, state.curr_t);
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

/*template <typename T> struct sign {*/
/*  T arg;*/
/*  sign(T arg_) : arg(arg_) {};*/
/**/
/*  static auto events =*/
/*      arg::events + Events(WhenZero(ref(arg)) |*/
/*                           SetUp([](auto &state) { state.sign[0] = 1; }) |*/
/*                           SetDown([](auto &state) { state.sign[0] = -1;
 * }));*/
/**/
/*  auto operator()(const auto &state) { return state.sign[0]; }*/
/*};*/

/*template <typename T> struct sign {*/
/*  T arg;*/
/*  double value = 1;*/
/*  sign(T arg_) : arg(arg_) {};*/
/**/
/*  auto operator()(const auto &state) const { return value; }*/
/**/
/*  auto prev(const auto &state) const { return value; }*/
/**/
/*  void set_value(const auto &state) { value = sign(arg(state)); }*/
/**/
/*  void is_switching(const auto &state) { return arg() * arg.prev() < 0; }*/
/**/
/*  T event_function_derivative = &arg;*/
/*};*/

} // namespace State

/*
    Variable<0> x;
    Variable<1> Dx;

    LHS lhs(Dx, sign(-x));

    if (lhs.is_switch_detected(state)) {
        auto switching_part = lhs.swiching_part();
        find
    }


 */
