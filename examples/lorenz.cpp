#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

struct Lorenz : diffurch::Solver<Lorenz> {

  // parameters in equation
  double sigma, rho, beta;

  Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  // define the right-hand side of the equation
  auto get_rhs() {
    using namespace diffurch::variables_xyz_t; // defines symbols x,y,z, and t
    return sigma * (y - x) | x * (rho - z) - y | x * y - beta * z;
  }
  //
  // define an initial condition (this can also be parametrized)
  auto get_ic() { return diffurch::Constant(1.1) | 1.2 | 20.; }
};

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  Lorenz eq;

  // compute the solution from t=0 to t=50, with stepsize 0.005
  auto sol = eq.solution(0, 50, diffurch::ConstantStepsize(0.005));
  auto [t, x, y, z] = sol; // unpack the tuple of vectors

  plt::plot(x, z, {{"color", "b"}, {"linewidth", "1"}});
  plt::tight_layout();
  plt::show();

  return 0;
}
