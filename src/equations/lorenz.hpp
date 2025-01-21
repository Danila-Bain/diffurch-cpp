#include "../solver.hpp"

struct Lorenz : Solver<Lorenz> {

  double sigma, rho, beta;

  Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto y = State::Variable<1>();
  static constexpr auto z = State::Variable<2>();

  static const bool ic_is_true_solution = false;

  auto get_lhs() {
    return State::Vector(sigma * (x - y), x * (rho - z) - y, x * y - beta * z);
  }
  auto get_ic() { return State::Vector(); }

  auto get_events() { return Events(StepEvent(std::make_tuple(t, x, y, z))); }
};
