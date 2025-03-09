#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

namespace plt = matplotlibcpp;
using namespace diffurch;
using namespace diffurch::variables_x_t;

struct EQ : Solver<EQ> {
  double b;
  double c;
  double d;
  double tau;
  auto get_rhs() { return Dx | -b * Dx - c * x + d * dsign(x(t - tau)); }
  auto get_ic() { return Vector(t, D(t)); }
};

int main(int, char *[]) {

  // EQ eq{.b = -0.02, .c = 1, .d = 1, .tau = 1};
  EQ eq{.b = -0.2, .c = 1, .d = 1, .tau = 1};
  auto sol = eq.solution(0, 5000, diffurch::ConstantStepsize(0.1));
  auto [t, x, dx] = sol;

  plt::plot(x, dx, {{"color", "b"}, {"linewidth", "0.1"}});
  plt::tight_layout();
  plt::show();

  return 0;
}
