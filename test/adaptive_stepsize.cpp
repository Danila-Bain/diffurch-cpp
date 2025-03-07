#include "../diffurch.hpp" // for solving
#include <matplot/util/common.h>

using namespace diffurch;

#include "../src/util/print.hpp"

#include <matplotlibcpp.h> // for plotting
#include <tuple>

namespace plt = matplotlibcpp;
using namespace diffurch::variables_xyz_t; // defines symbols x,y,z, and t
using namespace diffurch;

int main(int, char *[]) {

  {
    diffurch::equation::LogLinearODE2Complex eq(0., 1., 5000., 1.);
    auto sol = eq.solution(0, 6.28, diffurch::ConstantStepsize(0.1));
    auto [t, x, y] = sol;
    plt::loglog(x, y);
    plt::scatter(x, y, 20, {{"color", "black"}});
  }

  {
    diffurch::equation::LogLinearODE2Complex eq(0., 1., 10000., 1.);
    auto sol = eq.solution<rktp64>(
        0, 6.28, diffurch::AdaptiveStepsize(),
        std::make_tuple(StepEvent(t | x | y), RejectEvent(t | x | y)));
    auto [t, x, y, t_r, x_r, y_r] = sol;
    plt::loglog(x, y);
    plt::scatter(x, y, 20, {{"color", "black"}});
    plt::scatter(x_r, y_r, 20, {{"color", "red"}});
  }

  {
    diffurch::equation::LogLinearODE2Complex eq(0., 1., 20000., 1.);
    auto sol = eq.solution<rk98>(
        0, 6.28, diffurch::AdaptiveStepsize(),
        std::make_tuple(StepEvent(t | x | y), RejectEvent(t | x | y)));
    auto [t, x, y, t_r, x_r, y_r] = sol;
    plt::loglog(x, y);
    plt::scatter(x, y, 20, {{"color", "black"}});
    plt::scatter(x_r, y_r, 20, {{"color", "red"}});
  }

  plt::tight_layout();
  plt::show();

  return 0;
}
