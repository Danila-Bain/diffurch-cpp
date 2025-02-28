#pragma once
#include "../solver.hpp"
namespace diffurch::equation {
struct RelayDDE1 : Solver<RelayDDE1> {

  double alpha; // sign coeff
  double k;     // linear coeff
  double tau;   // delay

  RelayDDE1(double alpha_, double k_, double tau_)
      : alpha(alpha_), k(k_), tau(tau_) {};

  static constexpr auto t = TimeVariable();
  static constexpr auto x = Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_rhs() { return Vector(k * x + alpha * sign(x(t - tau))); }
  auto get_ic() {
    double a, b;
    if (k == 0) {
      a = -tau;
      b = 3 * tau;
      return Periodic(a, b,
                      Piecewise(t < tau, -alpha * t, alpha * (t - 2 * tau)));
    } else {
      a = tau + log(-1 + 2 * exp(-k * tau)) / k;
      b = tau - log(-1 + 2 * exp(-k * tau)) / k;
      return Periodic(
          a, b,
          Piecewise(t < tau, alpha / k * (1 - exp(k * t)),
                    -alpha / k * (1 + exp(k * t) * (1 - 2 * exp(-k * tau)))));
    }
  }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};
} // namespace diffurch::equation
