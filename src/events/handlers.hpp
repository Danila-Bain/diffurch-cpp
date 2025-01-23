#pragma once

#include "../symbolic/variables.hpp"
#include <cstddef>
#include <limits>
#include <tuple>
// Special Set Handler
struct StopIntegration {

  void operator()(auto &state) {
    state.t_prev = state.t_curr;
    state.t_curr = std::numeric_limits<double>::max();
  }
};

// Special Save Handler to save current time and state variables
// Intended to be part of default event for solver
template <typename Equation> auto SaveAll() {
  static constexpr size_t n =
      std::tuple_size<decltype(std::declval<Equation>.get_ic()(0.))>::value;

  return std::tuple_cat(std::make_tuple(State::TimeVariable()),
                        State::Variables<n>());
};
