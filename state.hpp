#pragma once

#include "equation.hpp"
#include "vec.hpp"
#include <algorithm>
#include <vector>

namespace State {

struct StateClassPlaceHolder {
  double curr_t;
  std::array<double, 0> curr_x;
  double prev_t;
  std::array<double, 0> prev_x;
  template <size_t derivative_order, size_t index = -1>
  auto eval(double t) const {
    if constexpr (index == -1) {
      return curr_x;
    } else {
      return curr_x[index];
    }
  }
};

template <Solvable Equation, RK_Table RK> struct State {
  static constexpr size_t n = order_of_equation<Equation>::value;

  // independent variable
  double curr_t;
  double prev_t;
  std::vector<double> t_sequence;

  // dependent variable
  Vec<n> curr_x;
  Vec<n> prev_x;
  vector<Vec<n>> x_sequence;

  // K values for interpolation, runge kutta method must support intrpolation
  // in the future, queue should be used instead of vector
  vector<array<Vec<n>, RK::s>> K_sequence;

  Equation *eq_ptr;

  State(double initial_time, Equation *eq_ptr_) {
    eq_ptr = eq_ptr_;

    curr_t = prev_t = initial_time;
    t_sequence.push_back(curr_t);

    curr_x = prev_x = eq_ptr->initial_condition(initial_time);
    x_sequence.push_back(curr_x);
  }

  template <size_t derivative_order = 0, size_t index = -1>
  auto eval(double t) {
    if (t <= t_sequence[0]) { // initial_condition case
      // here we separate two cases, because it is rare that we need to define
      // the derivative of the initial condition, in which case
      // initial_condition is defined as a template instead of a regular member
      // function.
      if constexpr (derivative_order == 0)
        return eq_ptr->initial_condition(t);
      else
        return eq_ptr->initial_condition<derivative_order>(t);
    } else {
      // t_sequence[i] would be the first element > t
      int i = std::distance(
          t_sequence.begin(),
          std::upper_bound(t_sequence.begin(), t_sequence.end(), t));
      double h = t_sequence[i] - t_sequence[i - 1];
      double theta = (t - t_sequence[i - 1]) / h;

      static_assert(
          requires { RK::eval<derivative_order>(K_sequence[i - 1], theta); },
          "Chosen Runge-Kutta method doesn't support interpolation.");

      if constexpr (index == -1)
        return RK::eval<derivative_order>(K_sequence[i - 1], theta);
      else
        return RK::eval<derivative_order>(K_sequence[i - 1][index], theta);
    }
  }
};
} // namespace State
