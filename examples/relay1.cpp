#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  diffurch::equation::Relay1 eq;

  // compute the solution from t=0 to t=50, with stepsize 0.005
  auto sol = eq.solution(-1, 1, diffurch::ConstantStepsize(0.5));
  auto [t, x] = sol; // unpack the tuple of vectors

  auto true_x = t;
  std::transform(t.begin(), t.end(), true_x.begin(),
                 [f = eq.get_ic()](double t_) { return f(t_)[0]; });
  plt::plot(t, x, {{"color", "b"}, {"linewidth", "1"}});
  plt::plot(t, true_x, {{"color", "k"}, {"linewidth", "1"}});
  plt::tight_layout();
  plt::show();

  return 0;
}
