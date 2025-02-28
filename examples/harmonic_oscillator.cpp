#include "../diffurch.hpp"
#include "../src/util/print.hpp"
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <tuple>

#include <matplot/matplot.h>

using namespace matplot;
using namespace diffurch;
using namespace diffurch::variables_x_t;

struct HarmonicOscillator : Solver<HarmonicOscillator> {

  double k;
  HarmonicOscillator(double k_ = 1.) : k(k_) {};

  auto get_lhs() { return Dx | -k * k * x; }
  auto get_ic() { return sin(k * t) | k * cos(k * t); }
  auto get_events() { return std::make_tuple(StepEvent(t | x | Dx)); }
};

int main(int, char *[]) {

  HarmonicOscillator eq(0.5);

  auto sol = eq.solution(0, 10, ConstantStepsize(0.05), std::make_tuple());
  auto [t, x, Dx] = sol;

  plot(t, x)->line_width(2.).color("red");
  hold(on);
  plot(t, Dx)->line_width(2.).color("blue");

  std::ofstream pout("../harmonic_oscillator.params");
  pout << "# " << "Parameters for harmonic_oscillator" << "\n";
  pout << "k = " << eq.k << "\n";
  pout << "stepsize = 0.05" << "\n";

  std::ofstream fout("../harmonic_oscillator.dat");

  fout << "# " << "Data points for harmonic_oscillator" << "\n";

  fout << "t x dx" << "\n";

  std::apply(
      [&](const auto &...v) {
        for (int i = 0; i < t.size(); i++) {
          ((fout << v[i] << " "), ...);
          fout << "\n";
        }
      },
      sol);

  /*for (int i = 0; i < t.size(); i++) {*/
  /**/
  /*}*/

  show();
  return 0;
}
