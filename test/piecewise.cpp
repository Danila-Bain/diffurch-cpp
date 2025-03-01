#include "../diffurch.hpp"
#include "../src/equations.hpp"
#include "../src/rk_tables/rktp64.hpp"
#include "../src/test/global_error.hpp"
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

struct SignRHS : Solver<SignRHS> {

  double c;
  double d;
  double x0;

  SignRHS(double c_, double d_, double x0_) : c(c_), d(d_), x0(x0_) {};

  auto get_rhs() { return Vector(c + d * dsign(x)); }
  auto get_ic() { return Vector(Constant(x0)); } // x(0) = x0
};

int main(int, char *[]) {

  {
    auto eq = SignRHS(2., 1., -1.);
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << sol << std::endl; //
  }
  {
    auto eq = SignRHS(-2., 1., 1.);
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << sol << std::endl; //
  }
  return 0;
}
