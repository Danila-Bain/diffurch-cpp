#pragma once
#include "../solver.hpp"
#include <cassert>

namespace diffurch::equation {
struct Relay1 : Solver<Relay1> {

  double a;
  double b;

  Relay1(double a_ = 2.1, double b_ = 1.2) : a(a_), b(b_) {
    assert(a > 0 && b > 0);
  };

  static constexpr auto t = TimeVariable();
  static constexpr auto x = Variable<0>();

  static const bool ic_is_true_solution = true;

  auto get_rhs() { return Vector(a + (b - a) * dstep(x)); }
  auto get_ic() { return Vector(a * t + (b - a) * relu(t)); }

  std::string repr(bool latex = true) {
    if (latex)
      return "$\\dot x = " + std::format("{:.3g}", a) + " + " +
             std::format("{:.3g}", b - a) + "H(t)$";
    else
      return "$x' = " + std::format("{:.3g}", a) + " + " +
             std::format("{:.3g}", b - a) + "H(t)$";
  }
};

// struct Relay2 : Solver<Relay2> {
//
//   double T;
//
//   Relay2(double T_ = 8) : T(T_) {};
//
//   static constexpr auto t = TimeVariable();
//   static constexpr auto x = Variable<0>();
//   static constexpr auto Dx = Variable<1>();
//
//   static const bool ic_is_true_solution = true;
//
//   auto get_rhs() { return Vector(Dx, -dsign(x)); }
//   auto get_ic() {
//     return periodic(-0.5 * T, 0.5 * T,
//                     piecewise(t < 0.,
//                              Vector(0.5 * t * (T * 0.5 + t), 0.25 * T + t),
//                              Vector(0.5 * t * (T * 0.5 - t), 0.25 * T - t)));
//   }
//
//   /*auto get_events() { return Events(StepEvent(std::make_tuple(t, x, Dx)));
//   }*/
// };
} // namespace diffurch::equation
