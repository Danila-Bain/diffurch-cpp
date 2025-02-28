#include "../diffurch.hpp"
#include "../src/equations.hpp"
#include "../src/rk_tables/rktp64.hpp"
#include "../src/util/print.hpp"
/*#include <iostream>*/
/*#include <cmath>*/
/*#include <matplot/matplot.h>*/

#include <matplotlibcpp.h>
#include <numpy/arrayobject.h>
#include <tuple>

namespace plt = matplotlibcpp;
using namespace diffurch;
using namespace variables_x_t;

int main(int, char *[]) {

  double tau = 0.5;
  // auto eq = equation::LinearNDDE1Sin(0.5, 0.5);
  auto eq = equation::LinearNDDE1Sin(0.5, tau);
  std::cout << eq.repr() << std::endl;

  auto sol = eq.solution(
      0., 5., ConstantStepsize(0.1),
      std::make_tuple(StepEvent(t | x | x(t - tau) | D(x)(t - tau))));

  auto [tt, xx, xd, dxd] = sol;
  std::cout << tt << std::endl;

  std::vector<double> true_x(tt.size());
  std::vector<double> true_xd(tt.size());

  std::transform(tt.begin(), tt.end(), true_x.begin(),
                 [true_sol = eq.get_ic()](double t) { return true_sol(t)[0]; });

  std::transform(
      tt.begin(), tt.end(), true_xd.begin(),
      [true_sol = eq.get_ic(), tau](double t) { return true_sol(t - tau)[0]; });

  plt::named_plot("x", tt, xx);
  plt::named_plot("x(t-tau)", tt, xd);
  plt::named_plot("x'(x-tau)", tt, dxd);
  // plt::scatter(t, x);
  plt::named_plot("true x", tt, true_x);
  plt::named_plot("true x(t-tau)", tt, true_xd);
  // auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  // std::cout << err << std::endl;
  // plt::named_loglog(eq.repr(), stepsizes, err);

  // plt::ylim(1.e-17, 1.e-3);
  plt::legend();
  plt::tight_layout();
  plt::show();

  return 0;
}
