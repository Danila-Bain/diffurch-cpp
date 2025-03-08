#pragma once
#include "util/vec.hpp"
#include <iostream>

#include "symbolic.hpp"
#include "symbolic/expression.hpp"
#include <algorithm>
#include <vector>

namespace diffurch {

template <typename RK, typename ICType> struct State {

  static constexpr size_t n =
      std::tuple_size<decltype(std::declval<ICType>()(0.))>::value;

  // independent variable
  double t_init;
  double t_curr;
  double t_prev;
  double t_step;
  // should be a queue, it used only for interpolation
  std::vector<decltype(t_curr)> t_sequence;

  // dependent variable
  ICType x_init;
  Vec<n> x_curr;
  Vec<n> x_prev;
  // should be a queue, used only for interpolation
  std::vector<decltype(x_curr)> x_sequence;

  // K values for interpolation, runge kutta method must support intrpolation
  // in the future, queue should be used instead of vector, and the requred time
  // span of that queue (i.e. the delay time, or )
  std::array<decltype(x_curr), RK::s> K_curr;
  std::vector<decltype(K_curr)> K_sequence;

  Vec<n> error_curr;

  State(double t_init, ICType x_init)
      : t_init(t_init), t_curr(t_init), t_prev(t_curr), t_sequence({t_curr}),
        x_init(x_init), x_curr(x_init(t_init)), x_prev(x_curr),
        x_sequence({x_curr}) {};

  void push_back_curr() {
    t_sequence.push_back(t_curr);
    x_sequence.push_back(x_curr);
    K_sequence.push_back(K_curr);

    // pop_front from the queues until t_sequence[1] > t_curr - t_span;
  }

  void make_zero_step() {
    // t_step is not updated, because it is the length of next step
    t_prev = t_curr;
    x_prev = x_curr;
    K_curr = decltype(K_curr){};
    push_back_curr();
  }

  template <size_t derivative_order = 0> decltype(x_curr) eval(double t) const {
    if (t <= t_sequence[0]) { // initial_condition case
      // here we separate two cases, because it is rare that we need to define
      // the derivative of the initial condition, in which case
      // initial_condition is defined as a template instead of a regular member
      // function.
      if constexpr (derivative_order == 0)
        return x_init(t);
      else if constexpr (IsStateExpression<decltype(x_init)>) {
        static const auto x_init_derivative = D<derivative_order>(x_init);
        return x_init_derivative(t);
      } else { // fallback for non-symbolic initial condition functions
        return x_init.template eval<derivative_order>(t);
      }
    } else if (t >= t_prev && t <= t_curr) {
      double h = t_curr - t_prev;
      double theta = (t - t_prev) / h;

      auto result =
          dot(eval_array<derivative_order>(RK::bs, theta), K_curr, RK::s);
      if constexpr (derivative_order == 0)
        result = x_prev + h * result;
      else
        result = pow(h, 1 - derivative_order) * result;
      return result;
    } else {
      // t_sequence[i] would be the first element > t
      // std::cout << i << "/" << t_sequence.size() << std::endl;
      int i = std::distance(
          t_sequence.begin(),
          std::upper_bound(t_sequence.begin(), t_sequence.end(), t));
      double h = t_sequence[i] - t_sequence[i - 1];
      double theta = (t - t_sequence[i - 1]) / h;

      auto result = dot(eval_array<derivative_order>(RK::bs, theta),
                        K_sequence[i - 1], RK::s);
      if constexpr (derivative_order == 0)
        result = x_sequence[i - 1] + h * result;
      else
        result = pow(h, 1 - derivative_order) * result;
      return result;
    }
  }
};

} // namespace diffurch
