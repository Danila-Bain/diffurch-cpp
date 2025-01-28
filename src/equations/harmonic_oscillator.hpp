#pragma once

#include "../solver.hpp"

namespace Equation {

struct HarmonicOscillator : Solver<HarmonicOscillator> {

  double w;

  HarmonicOscillator(double w_ = 1.) : w(w_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() { return Dx | -w * w * x; }
  auto get_ic() { return sin(w * t) | w * cos(w * t); }

  std::string repr(bool latex = true) {
    std::string w2 = (w * w == 1 ? "" : std::format("{:.3g}", w * w));
    if (latex)
      return "$\\ddot x = " + w2 + " x$";
    else
      return "x'' = " + w2 + "x";
  }
  /*auto get_lhs() { return State::Vector(Dx, -w * w * x); }*/
  /*auto get_ic() { return State::Vector(sin(w * t), w * cos(w * t)); }*/
  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x, Dx))); }*/
  /*auto get_events() { return Events(StepEvent(t | x | Dx)); }*/
};
} // namespace Equation
