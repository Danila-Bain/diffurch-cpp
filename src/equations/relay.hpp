#include "../solver.hpp"

struct Relay1 : Solver<Relay1> {

  double a;
  double b;

  Relay1(double a_ = 2, double b_ = 1) : a(a_), b(b_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    double A = (a + b) / 2;
    double B = (a - b) / 2;
    return State::Vector(A + B * sign(x));
  }
  auto get_ic() {
    double A = (a + b) / 2;
    double B = (a - b) / 2;
    return State::Vector(A * t + B * abs(t));
  }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};

struct Relay2 : Solver<Relay2> {

  double T;

  Relay2(double T_ = 8) : T(T_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() { return State::Vector(Dx, -sign(x)); }
  auto get_ic() {
    return Periodic(-0.5 * T, 0.5 * T,
                    Piecwise(t < 0,
                             Vector(0.5 * t * (T * 0.5 + t), 0.25 * T + t),
                             Vector(0.5 * t * (T * 0.5 - t), 0.25 * T - t)));
  }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x, Dx))); }
};
