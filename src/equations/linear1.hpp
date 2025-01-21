#include "../solver.hpp"

struct Linear1 : Solver<Linear1> {

  double k;

  Linear1(double k_ = -1.) : k(k_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() { return State::Vector(k * x); }
  auto get_ic() { return State::Vector(exp(k * t)); }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};
