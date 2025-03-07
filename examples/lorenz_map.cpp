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
  // define an initial condition (this can also be parametrized)
  auto get_ic() { return diffurch::Constant(1.1) | 1.2 | 20.; }
};

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  Lorenz eq;

  using namespace diffurch::variables_xyz_t; // defines symbols x,y,z, and t
  using namespace diffurch;

  auto sol =
      eq.solution(0, 500, diffurch::ConstantStepsize(0.01),
                  std::make_tuple(Event(When(x * y - eq.beta * z < 0), t | z)));
  auto [tmax, zmax] = sol; // unpack the tuple of vectors

  plt::scatter(std::vector(zmax.begin(), zmax.end() - 1),
               std::vector(zmax.begin() + 1, zmax.end()));

  plt::plot(zmax, zmax);

  plt::tight_layout();
  plt::show();

  return 0;
}
