#include "../diffurch.hpp"
#include <iostream>

using namespace diffurch;
using namespace diffurch::variables_x_t;

struct HarmonicOscillator : Solver<HarmonicOscillator> {
  auto get_lhs() { return Dx | -x; }
  auto get_ic() { return sin(t) | cos(t); }
};

int main(int, char *[]) {

  HarmonicOscillator eq;

  auto [t, x, Dx] = eq.solution(0, 10);

  return 0;
}
