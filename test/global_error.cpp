#include "../src/test/global_error.hpp"
#include "../diffurch.hpp"
#include "../src/equations.hpp"
#include "../src/rk_tables/rktp64.hpp"
#include "../src/util/print.hpp"
/*#include <iostream>*/
/*#include <cmath>*/
/*#include <matplot/matplot.h>*/

#include <matplotlibcpp.h>
#include <numpy/arrayobject.h>

namespace plt = matplotlibcpp;
using namespace diffurch;

int main(int, char *[]) {

  /*Equation::HarmonicOscillator eq;*/
  /*Equation::Linear1 eq;*/
  /*auto true_solution = eq.get_ic();*/
  /**/
  std::vector<double> stepsizes = logspace(0.01, 1, 500);

  {
    auto eq = equation::HarmonicOscillator();
    auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
    std::cout << err << std::endl;
    plt::named_loglog(eq.repr(), stepsizes, err);
  }
  /*{*/
  /*  auto eq = Equation::Linear1();*/
  /*  auto err = global_error<rk98>(eq, 0, 10, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/
  /*{*/
  /*  auto eq = Equation::LogOscillator();*/
  /*  auto err = global_error<rk98>(eq, 0, 10, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/
  /*{*/
  /*  auto eq = Equation::LinearDDE1Exp();*/
  /*  auto err = global_error<rk98>(eq, 0, 10, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/
  /*{*/
  /*  auto eq = equation::LinearDDE1Sin();*/
  /*  auto err = diffurch::test::global_error<rk98>(eq, 0, 10, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/
  /*{*/
  /*  auto eq = Equation::LinearDDE2Sin();*/
  /*  auto err = global_error<rk98>(eq, 0, 10, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/
  /*{*/
  /*  auto eq = equation::LinearNDDE1Sin();*/
  /*  auto err = test::global_error<rk98>(eq, 0, 40, stepsizes);*/
  /*  std::cout << err << std::endl;*/
  /*  plt::named_loglog(eq.repr(), stepsizes, err);*/
  /*}*/

  plt::ylim(1.e-17, 1.e-3);
  plt::legend();
  plt::tight_layout();
  plt::show();

  return 0;
}
