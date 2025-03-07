#pragma once

#include "../../solver.hpp"
#include <math.h>

namespace diffurch::equation {
struct LinearODE1 : Solver<LinearODE1> {

  double k;
  double c;

  LinearODE1(double k_ = -1., double c_ = 1.) : k(k_), c(c_) {};

  static constexpr auto t = TimeVariable();
  static constexpr auto x = Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_rhs() { return Vector(k * x); }
  auto get_ic() { return Vector(c * exp(k * t)); }

  std::string repr(bool latex = true) {
    std::string kk = (k == 1 ? "" : k == -1 ? "-" : std::format("{:.3g}", k));
    if (latex)
      return "$\\dot x = " + kk + " x$";
    else
      return "x' = " + kk + "x";
  }
};

// the solutions of this equation are related to solutions of LinearODE2Complex
// equation by the change of variables x_log = exp(x_lin) and y_lin = exp(y_log)
// this equation is supposed to test the performance of numerical method, when
// the derivative of the solution varies drastically during integration.
struct LogLinearODE2Complex : Solver<LogLinearODE2Complex> {

  double re;
  double im;
  double c1;
  double c2;

  LogLinearODE2Complex(double re_, double im_, double c1_, double c2_)
      : re(re_), im(im_), c1(c1_), c2(c2_) {}

  static constexpr auto t = TimeVariable();
  static constexpr auto x = Variable<0>();
  static constexpr auto y = Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_rhs() {
    return (re * log(x) + im * log(y)) * x | (-im * log(x) + re * log(y)) * y;
  }
  auto get_ic() {
    using std::log;
    return exp(exp(re * t) * (+log(c1) * cos(im * t) + log(c2) * sin(im * t))) |
           exp(exp(re * t) * (-log(c1) * sin(im * t) + log(c2) * cos(im * t)));
  }
};

} // namespace diffurch::equation
