#pragma once

#include <utility>

/*template <typename Equation>*/
/*concept Solvable = requires(Equation eq, double t) {*/
/*  {*/
/*    std::is_same_v<decltype(eq.initial_condition(t)),*/
/*                   decltype(eq.lhs(State(t, &eq)))>*/
/*  };*/
/*};*/

template <typename Equation> struct order_of_equation {
  static constexpr std::size_t value =
      std::tuple_size<decltype(std::declval<Equation>().get_ic()(0.))>::value;
};
