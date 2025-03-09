#pragma once

#include "../util/math.hpp"
#include "detect_symbols.hpp"
#include "symbol_types.hpp"
#include <cstddef>
#include <tuple>

namespace diffurch {

template <IsSymbol Arg> struct dsign : Symbol {
  Arg arg;
  dsign(Arg arg_) : arg(arg_) {}
  double curr_value = 0;
  auto operator()(const auto &state) const { return curr_value; }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(arg.template get_events<current_coordinate>(),
                          std::make_tuple(StartEvent(nullptr,
                                                     [this](const auto &state) {
                                                       curr_value =
                                                           sign(arg(state));
                                                     }),
                                          Event(When(arg == 0), nullptr,
                                                [this](const auto &state) {
                                                  curr_value = -curr_value;
                                                })));
  }
};
template <std::size_t derivative = 1, IsSymbol Arg>
constexpr auto D(const dsign<Arg> &sign_) {
  if constexpr (derivative == 0)
    return sign_;
  else
    return D<derivative - 1>(2 * delta(sign_.arg) * D(sign_.arg));
}

template <IsSymbol Arg> struct dstep : Symbol {
  Arg arg;
  const double low_value;
  const double high_value;

  dstep(Arg arg_, double low = 0, double high = 1)
      : arg(arg_), low_value(low), high_value(high) {}

  double curr_value;

  auto operator()(const auto &state) const { return curr_value; }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        arg.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         curr_value = step(arg(state), low_value, high_value);
                       }),
            Event(When(arg == 0), nullptr, [this](const auto &state) {
              curr_value = step(arg(state), low_value, high_value);
            })));
  }
};
template <std::size_t derivative = 1, IsSymbol Arg>
constexpr auto D(const dstep<Arg> &step_) {
  if constexpr (derivative == 0)
    return step_;
  else
    return D<derivative - 1>((step_.high_value - step_.low_value) *
                             delta(step_.arg) * D(step_.arg));
}

template <IsSymbol Arg> struct dabs : Symbol {
  Arg arg;

  dabs(Arg arg_) : arg(arg_) {}

  double curr_sign = 1;

  auto operator()(const auto &state) const { return curr_sign * arg(state); }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(arg.template get_events<current_coordinate>(),
                          std::make_tuple(StartEvent(nullptr,
                                                     [this](const auto &state) {
                                                       curr_sign =
                                                           sign(arg(state));
                                                     }),
                                          Event(When(arg == 0), nullptr,
                                                [this](const auto &state) {
                                                  curr_sign = -curr_sign;
                                                })));
  }
};
template <std::size_t derivative = 1, IsSymbol Arg>
constexpr auto D(const dabs<Arg> &abs_) {
  if constexpr (derivative == 0)
    return abs_;
  else
    return D<derivative - 1>(dsign(abs_.arg) * D(abs_.arg));
}

template <IsSymbol Arg> struct drelu : Symbol {
  Arg arg;

  drelu(Arg arg_) : arg(arg_) {}

  double curr_mask = 1;

  auto operator()(const auto &state) const { return curr_mask * arg(state); }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        arg.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(
                nullptr,
                [this](const auto &state) { curr_mask = step(arg(state)); }),
            Event(When(arg == 0), nullptr, [this](const auto &state) {
              curr_mask = 1. - curr_mask; // 1 -> 0; 0->1
            })));
  }
};
template <std::size_t derivative = 1, IsSymbol Arg>
constexpr auto D(const drelu<Arg> &relu_) {
  if constexpr (derivative == 0)
    return relu_;
  else
    return D<derivative - 1>(dstep(relu_.arg) * D(relu_.arg));
}

template <IsBoolSymbol Condition, IsSymbol ExprIfTrue, IsSymbol ExprIfFalse>
struct dpiecewise : Symbol {
  Condition condition;
  ExprIfTrue expr_if_true;
  ExprIfFalse expr_if_false;

  dpiecewise(Condition condition_, ExprIfTrue expr_if_true_,
             ExprIfFalse expr_if_false_)
      : condition(condition_), expr_if_true(expr_if_true_),
        expr_if_false(expr_if_false_) {};

  bool condition_value;

  auto operator()(const auto &state) const {
    return condition_value ? expr_if_true(state) : expr_if_false(state);
  }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        expr_if_true.template get_events<current_coordinate>(),
        expr_if_false.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         condition_value = condition(state);
                       }),
            Event(WhenSwitch(condition), nullptr, [this](const auto &state) {
              condition_value = condition(state);
            })));
  }
};

template <IsSymbol Arg> struct dclip : Symbol {
  Arg arg;
  const double min_value;
  const double max_value;

  dclip(Arg arg_, double min_value_ = 0, double max_value_ = 1)
      : arg(arg_), min_value(min_value_), max_value(max_value_) {}

  size_t idx;

  auto operator()(const auto &state) const {
    switch (idx) {
    case 0:
      return min_value;
    case 1:
      return arg(state);
    case 2:
      return max_value;
    }
  }

  template <size_t current_coordinate = size_t(-1)> auto get_events() {
    return std::tuple_cat(
        arg.template get_events<current_coordinate>(),
        std::make_tuple(
            StartEvent(nullptr,
                       [this](const auto &state) {
                         auto val = arg(state);
                         idx = (val > min_value) + (val >= max_value);
                       }),
            Event(
                When(arg == min_value), nullptr,
                [this](const auto &state) { idx = (arg(state) > min_value); }),
            Event(When(arg == max_value), nullptr, [this](const auto &state) {
              idx = 1 + (arg(state) >= max_value);
            })));
  }
};

template <IsSymbol Arg> struct delta : Symbol {
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
