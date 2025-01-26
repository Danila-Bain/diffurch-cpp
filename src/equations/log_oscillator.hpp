#pragma once
#include "../solver.hpp"
namespace Equation {
struct LogOscillator : Solver<LogOscillator> {

  double w;

  LogOscillator(double w_ = 1.) : w(w_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    return State::Vector(Dx, w * w * x * (1 - log(x) - log(x) * log(x)));
  }
  auto get_ic() {
    return State::Vector(exp(sin(w * t)), w * cos(w * t) * exp(sin(w * t)));
  }

  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x, Dx))); }*/
};
} // namespace Equation
