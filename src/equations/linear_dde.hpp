#pragma once
#include "../solver.hpp"
namespace Equation {
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
    double a = k * (1 - theta);
    double b = k * exp(tau) * theta;
    return State::Vector(a * x + b * x(t - tau));
  }
  auto get_ic() { return State::Vector(exp(k * t)); }

  std::string repr(bool latex = true) {
    double a = k * (1 - theta);
    double b = k * exp(tau) * theta;
    std::string aa = std::format("{:.3g}", a);
    std::string bb = std::format("{:+.3g}", b);
    std::string tt = std::format("{:.3g}", tau);
    if (latex)
      return "$\\dot x = " + aa + " x " + bb + "x(t-" + tt + ")$";
    else
      return "$x' = " + aa + " x " + bb + "x(t-" + tt + ")$";
  }
  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }*/
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
    return State::Vector(a * x + b * x(t - tau));
  }
  auto get_ic() { return State::Vector(sin(k * t)); }

  std::string repr(bool latex = true) {
    double a = k / tan(k * tau);
    double b = -k / sin(k * tau);
    std::string aa = std::format("{:.3g}", a);
    std::string bb = std::format("{:+.3g}", b);
    std::string tt = std::format("{:.3g}", tau);
    if (latex)
      return "$\\dot x = " + aa + " x " + bb + "x(t-" + tt + ")$";
    else
      return "$x' = " + aa + " x " + bb + "x(t-" + tt + ")$";
  }
  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }*/
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
    return Dx | a * x + b * x(t - tau) + c * Dx(t - tau);
  }
  auto get_ic() { return sin(k * t) | k * cos(k * t); }

  std::string repr(bool latex = true) {
    double a = -k * k * (1 - theta);
    double b = -theta * k * k * cos(k * tau);
    double c = -theta * k * sin(k * tau);
    std::string aa = std::format("{:.3g}", a);
    std::string bb = std::format("{:+.3g}", b);
    std::string cc = std::format("{:+.3g}", c);
    std::string tt = std::format("{:.3g}", tau);
    if (latex)
      return "$\\ddot x = " + aa + " x " + bb + "x(t-" + tt + ") " + cc +
             "\\dot x(t-" + tt + ")$";
    else
      return "$x'' = " + aa + " x " + bb + "x(t-" + tt + ") " + cc + "x'(t-" +
             tt + ")$";
  }

  /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x))); }*/
};

struct LinearNDDE1Sin : Solver<LinearNDDE1Sin> {

  double k;   // exponential rate
  double tau; // delay amount

  LinearNDDE1Sin(double k_ = 1, double tau_ = 1) : k(k_), tau(tau_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<0, 1>();

  static const bool ic_is_true_solution = true;

  auto get_lhs() {
    double a = -k * tan(k * tau);
    double b = k / cos(k * tau);
    return State::Vector(a * x + b * Dx(t - tau));
  }
  auto get_ic() { return State::Vector(sin(k * t)); }

  std::string repr(bool latex = true) {
    double a = -k * tan(k * tau);
    double b = k / cos(k * tau);
    std::string aa = std::format("{:.3g}", a);
    std::string bb = std::format("{:+.3g}", b);
    std::string tt = std::format("{:.3g}", tau);
    if (latex)
      return "$\\dot x = " + aa + " x " + bb + "\\dot x(t-" + tt + ")$";
    else
      return "$x' = " + aa + " x " + bb + "x'(t-" + tt + ")$";
  }
};
} // namespace Equation
