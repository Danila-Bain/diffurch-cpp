#pragma once
#include <iostream>

#include "equation.hpp"
#include "vec.hpp"
#include <algorithm>
#include <vector>

namespace State {

/*struct StateClassPlaceHolder {*/
/*  double t_curr;*/
/*  std::array<double, 0> x_curr;*/
/*  double t_prev;*/
/*  std::array<double, 0> x_prev;*/
/*  template <size_t derivative_order, size_t index = -1>*/
/*  auto eval(double t) const {*/
/*    if constexpr (index == -1) {*/
/*      return x_curr;*/
/*    } else {*/
/*      return x_curr[index];*/
/*    }*/
/*  }*/
/*};*/

template <typename RK, typename InitialConditionHandler> struct State {

  static constexpr size_t n =
      std::tuple_size<decltype(std::declval<InitialConditionHandler>()(
          0.))>::value;

  InitialConditionHandler ic;
  // independent variable
  double t_curr;
  double t_prev;
  std::vector<double> t_sequence;

  // dependent variable
  Vec<n> x_curr;
  Vec<n> x_prev;
  vector<Vec<n>> x_sequence;

  // K values for interpolation, runge kutta method must support intrpolation
  // in the future, queue should be used instead of vector
  array<Vec<n>, RK::s> K_curr;
  vector<array<Vec<n>, RK::s>> K_sequence;

  /*Equation *eq_ptr;*/

  State(double initial_time, InitialConditionHandler ic_)
      : ic(ic_), t_curr(initial_time), t_prev(t_curr), t_sequence({t_curr}),
        x_curr(ic(initial_time)), x_prev(x_curr), x_sequence({x_curr}) {};

  void push_back_curr() {
    t_sequence.push_back(t_curr);
    x_sequence.push_back(x_curr);
    K_sequence.push_back(K_curr);
    /*std::cout << x_sequence << std::endl << std::endl;*/
  }

  template <size_t derivative_order = 0, size_t index = -1>
  auto eval(double t) {
    if (t <= t_sequence[0]) { // initial_condition case
      // here we separate two cases, because it is rare that we need to define
      // the derivative of the initial condition, in which case
      // initial_condition is defined as a template instead of a regular member
      // function.
      if constexpr (derivative_order == 0)
        return ic(t);
      else
        return ic<derivative_order>(t);
    } else {
      // t_sequence[i] would be the first element > t
      int i = std::distance(
          t_sequence.begin(),
          std::upper_bound(t_sequence.begin(), t_sequence.end(), t));
      double h = t_sequence[i] - t_sequence[i - 1];
      double theta = (t - t_sequence[i - 1]) / h;

      static_assert(
          requires {
            RK::template eval<derivative_order>(K_sequence[i - 1], theta);
          }, "Chosen Runge-Kutta method doesn't support interpolation.");

      if constexpr (index == -1)
        return RK::template eval<derivative_order>(K_sequence[i - 1], theta);
      else
        return RK::template eval<derivative_order>(K_sequence[i - 1][index],
                                                   theta);
    }
  }
};

} // namespace State
