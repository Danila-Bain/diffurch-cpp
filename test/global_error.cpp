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
  std::vector<double> stepsizes = logspace(0.01, 0.5, 500);

  {
    auto eq = equation::HarmonicOscillator();
    auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
    std::cout << eq.repr() << std::endl;
    /*std::cout << err << std::endl;*/
    plt::named_loglog(eq.repr(), stepsizes, err);
  }
  // {
  //   auto eq = equation::Linear1(1.);
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   /*std::cout << err << std::endl;*/
  //   std::cout << eq.repr() << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  // {
  //   auto eq = equation::LogOscillator();
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   /*std::cout << err << std::endl;*/
  //   std::cout << eq.repr() << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  // {
  //   auto eq = equation::LinearDDE1Exp();
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   /*std::cout << err << std::endl;*/
  //   std::cout << eq.repr() << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  // {
  //   auto eq = equation::LinearDDE1Sin();
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   /*std::cout << err << std::endl;*/
  //   std::cout << eq.repr() << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  // {
  //   auto eq = equation::LinearDDE2Sin();
  //   std::cout << eq.repr() << std::endl;
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   // std::cout << err << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  // {
  //   auto eq = equation::LinearNDDE1Sin();
  //   std::cout << eq.repr() << std::endl;
  //   auto err = test::global_error<rk98>(eq, 0, 10, stepsizes);
  //   // std::cout << err << std::endl;
  //   plt::named_loglog(eq.repr(), stepsizes, err);
  // }
  {
    auto eq = equation::Relay1();
    std::cout << eq.repr() << std::endl;
    auto err = test::global_error<rk98>(eq, -10, 10, stepsizes);
    // std::cout << err << std::endl;
    plt::named_loglog(eq.repr(), stepsizes, err);
  }

  plt::ylim(1.e-17, 1.e-3);
  plt::legend();
  plt::tight_layout();
  plt::show();

  return 0;
}
