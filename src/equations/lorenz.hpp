#pragma once
#include "../solver.hpp"
#include <tuple>
namespace diffurch::equation {
struct Lorenz : Solver<Lorenz> {

  double sigma, rho, beta;

  Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  static const bool ic_is_true_solution = false;

  auto get_rhs() {
    using namespace diffurch::variables_xyz_t;
    return sigma * (y - x) | x * (rho - z) - y | x * y - beta * z;
  }
  auto get_ic() { return Constant(1.1) | 1.2 | 20.; }
};
} // namespace diffurch::equation
