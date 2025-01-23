#include "../src/equations/harmonic_oscillator.hpp"
#include "../src/util/print.hpp"
#include <iostream>
#include <matplot/matplot.h>

int main(int, char *[]) {

  HarmonicOscillator eq;

  auto true_solution = eq.get_ic();

  std::cout << true_solution(0.) << std::endl;

  std::cout << true_solution(3.141592 / 2) << std::endl;

  auto [t, x, y] = eq.solution(0., 10.);

  using namespace matplot;

  plot(t, x, "-o");
  hold(on);
  plot(t, y, "-o");

  show();

  return 0;
}
