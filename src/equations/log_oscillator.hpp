#pragma once
#include "../solver.hpp"
namespace diffurch::equation {
struct LogOscillator : Solver<LogOscillator> {

  double w;

  LogOscillator(double w_ = 0.5) : w(w_) {};

  static constexpr auto t = TimeVariable();
  static constexpr auto x = Variable<0>();
  static constexpr auto Dx = Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_rhs() {
    return Vector(Dx, w * w * x * (1 - log(x) - log(x) * log(x)));
  }
  auto get_ic() {
    return Vector(exp(sin(w * t)), w * cos(w * t) * exp(sin(w * t)));
  }

  std::string repr(bool latex = true) {
    std::string w2 = (w * w == 1 ? "" : std::format("{:.3g}", w * w));
    if (latex)
      return "$\\ddot x = " + w2 + " x (1 - \\ln(x) - \\ln(x)^2)$";
    else
      return "x'' = " + w2 + "x (1 - ln(x) - ln(x)^2)";
  }

  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x, Dx))); }*/
};
} // namespace diffurch::equation
