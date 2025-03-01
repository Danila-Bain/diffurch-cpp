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

struct tSignRHS : Solver<tSignRHS> {
  double c;
  double d;
  double x0;
  tSignRHS(double c_, double d_, double x0_) : c(c_), d(d_), x0(x0_) {};

  auto get_rhs() { return Vector(c + d * dsign(t)); }
  auto get_ic() { return Vector(Constant(x0)); }
};
struct tStepRHS : Solver<tSignRHS> {
  double c;
  double d;
  double x0;
  tStepRHS(double c_, double d_, double x0_) : c(c_), d(d_), x0(x0_) {};

  auto get_rhs() { return Vector(dstep(t, c, d)); }
  auto get_ic() { return Vector(Constant(x0)); }
};
struct tAbsRHS : Solver<tAbsRHS> {
  double c;
  double d;
  double x0;
  tAbsRHS(double c_, double d_, double x0_) : c(c_), d(d_), x0(x0_) {};

  auto get_rhs() { return Vector(c + d * dabs(t)); }
  auto get_ic() { return Vector(Constant(x0)); }
};

int main(int, char *[]) {

  {
    auto eq = SignRHS(2., 1., -1.); // x' = 2 + sign(x); x(0) = -1;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << "Sign (neg to pos):" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    auto eq = SignRHS(-2., 1., 1.); // x' = -2 + sign(x); x(0) = 1;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << "Sign (pos to neg):" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    auto eq = tSignRHS(2., 1., -1.); // x' = 2 + sign(t); x(0) = -1;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tSign:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    auto eq = tStepRHS(1., 2., -1.); // x' = t < 0 ? 1 : 2; x(0) = -1
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tStep:" << std::endl;
    std::cout << sol << std::endl; //
  }
  return 0;
}
