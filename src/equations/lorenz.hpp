#pragma once
#include "../solver.hpp"
#include <tuple>
namespace diffurch::equation {
struct Lorenz : Solver<Lorenz> {

  double sigma, rho, beta;

  Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  /*static constexpr auto t = TimeVariable();*/
  /*static constexpr auto x = Variable<0>();*/
  /*static constexpr auto y = Variable<1>();*/
  /*static constexpr auto z = Variable<2>();*/

  static const bool ic_is_true_solution = false;

  auto get_rhs() {
    using namespace diffurch::variables_xyz_t;
    return sigma * (y - x) | x * (rho - z) - y | x * y - beta * z;
  }
  auto get_ic() { return Constant(1.1) | 1.2 | 20.; }

  auto get_events() {
    using namespace diffurch::variables_xyz_t;
    return std::make_tuple(StepEvent(t | x | y | z));
  }
};
} // namespace diffurch::equation
