#pragma once

#include "../util/math.hpp"
#include "expression.hpp"
#include <iterator>
#include <tuple>

namespace diffurch {

template <IsStateExpression Arg> struct delta : StateExpression {
  Arg arg;
  delta(Arg arg_) : arg(arg_) {}

  auto operator()(const auto &state) const { return 0.; }
  auto operator()(const auto &state, double t) const { return 0.; }
  auto operator()(double t) const { return 0.; }
  auto prev(const auto &state) const { return 0.; }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        arg.get_events(),
        std::make_tuple(Event(When(arg == 0), nullptr, [this](auto &state) {
          state.x_curr[current_coordinate] =
              state.x_prev[current_coordinate] + 1 / abs(D(arg)(state));
        })));
  }
};

} // namespace diffurch
