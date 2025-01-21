#include "../solver.hpp"

struct LinearDDE1Exp : Solver<LinearDDE1Exp> {

  double theta; // 0 -> without delay, 1 -> only delay
  double k;     // exponential rate
  double tau;   // delay amount

  LinearDDE1Exp(double theta_ = 0.5, double k_ = 1, double tau_ = 1)
      : theta(theta_), k(k_), tau(tau_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    return State::Vector(k * (1 - theta) * x +
                         (k * exp(tau) * theta) * x[t - tau]);
  }
  auto get_ic() { return State::Vector(exp(k * t)); }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};

struct LinearDDE1Sin : Solver<LinearDDE1Sin> {

  double k;   // exponential rate
  double tau; // delay amount

  LinearDDE1Sin(double k_ = 1, double tau_ = 1) : k(k_), tau(tau_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    double a = k / tan(k * tau);
    double b = -k / sin(k * tau);
    return State::Vector(a * x + b * x[t - tau]);
  }
  auto get_ic() { return State::Vector(sin(k * t)); }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};

struct LinearDDE2Sin : Solver<LinearDDE2Sin> {

  double theta;
  double k;   // exponential rate
  double tau; // delay amount

  LinearDDE2Sin(double theta_ = 0.5, double k_ = 1, double tau_ = 1)
      : theta(theta_), k(k_), tau(tau_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<1>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    double a = -k * k * (1 - theta);
    double b = -theta * k * k * cos(k * tau);
    double c = -theta * k * sin(k * tau);
    return State::Vector(Dx, a * x + b * x[t - tau] + c * Dx[t - tau]);
  }
  auto get_ic() { return State::Vector(sin(k * t)); }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }
};
