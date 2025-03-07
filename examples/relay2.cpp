#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  diffurch::equation::Relay2 eq;

  auto sol = eq.solution(-0.001, 10, diffurch::ConstantStepsize(0.1));
  auto [t, x, dx] = sol;

  auto true_x = t;
  auto true_dx = t;
  std::transform(t.begin(), t.end(), true_x.begin(),
                 [f = eq.get_ic()](double t_) { return f(t_)[0]; });
  std::transform(t.begin(), t.end(), true_dx.begin(),
                 [f = eq.get_ic()](double t_) { return f(t_)[1]; });

  plt::plot(t, true_x, {{"color", "k"}, {"linewidth", "4"}});
  plt::plot(t, true_dx, {{"color", "k"}, {"linewidth", "4"}});
  plt::plot(t, x, {{"color", "r"}, {"linewidth", "2"}});
  plt::plot(t, dx, {{"color", "r"}, {"linewidth", "2"}});
  plt::tight_layout();
  plt::show();

  return 0;
}
