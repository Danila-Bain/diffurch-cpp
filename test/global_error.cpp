#include "../src/test/global_error.hpp"
#include "../src/equations/harmonic_oscillator.hpp"
#include "../src/util/print.hpp"
/*#include <iostream>*/
/*#include <cmath>*/
/*#include <matplot/matplot.h>*/

#include <matplotlibcpp.h>
#include <numpy/arrayobject.h>

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  HarmonicOscillator eq;
  auto true_solution = eq.get_ic();

  std::vector<double> stepsizes = logspace(0.1, 1, 500);
  std::vector<double> errors = global_error(eq, 0, 50, stepsizes);

  plt::loglog(stepsizes, errors);
  plt::show();

  /*    std::cout << stepsizes << std::endl;*/
  /*    std::cout << errors << std::endl;*/
  /*    loglog(stepsizes, errors, "-o");*/
  /*    std::cout << stepsizes << std::endl;*/
  /*    std::cout << errors << std::endl;*/
  /*    show();*/

  return 0;
}
