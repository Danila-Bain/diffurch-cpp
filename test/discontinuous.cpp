#include <iostream>

#include "../diffurch.hpp"
#include "../src/equations.hpp"
#include "../src/rk_tables/rktp64.hpp"
#include "../src/test/global_error.hpp"
#include "../src/util/print.hpp"
/*#include <cmath>*/
/*#include <matplot/matplot.h>*/

#include <matplotlibcpp.h>
#include <numpy/arrayobject.h>
#include <tuple>

namespace plt = matplotlibcpp;
using namespace diffurch;
using namespace variables_xyz_t;

int main(int, char *[]) {

  {
    struct SignRHS : Solver<SignRHS> {
      auto get_rhs() { return Vector(2. + 1. * dsign(x)); }
      auto get_ic() { return Vector(Constant(-1.)); }
    } eq;

    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << "Sign (neg to pos):" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct SignRHS : Solver<SignRHS> {
      auto get_rhs() { return Vector(-2. + 1. * dsign(x)); }
      auto get_ic() { return Vector(Constant(1.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(x)))));

    std::cout << "Sign (pos to neg):" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct tSignRHS : Solver<tSignRHS> {
      auto get_rhs() { return Vector(2. + dsign(t)); }
      auto get_ic() { return Vector(Constant(-1.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tSign:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct tStepRHS : Solver<tStepRHS> {
      auto get_rhs() { return Vector(dstep(t, 1., 2.)); }
      auto get_ic() { return Vector(Constant(-1.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.3),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tStep:" << std::endl;
    std::cout << sol << std::endl; //
  }

  {
    struct tAbsRHS : Solver<tAbsRHS> {
      auto get_rhs() { return Vector(dabs(t)); }
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.25),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tAbs:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct tAbsRHS : Solver<tAbsRHS> {
      auto get_rhs() { return Vector(dabs(t)); }
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.4),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tAbs:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct tAbsRHSBad : Solver<tAbsRHSBad> {
      auto get_rhs() { return Vector(abs(t)); } // here abs instead of dabs
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(
        -1, 1, ConstantStepsize(0.4),
        std::tuple_cat(std::make_tuple(StepEvent(t | x | sign(t)))));

    std::cout << "tAbs (Without detection) (there sould be x(1) = 1 but there "
                 "is not):"
              << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct tPiecewise : Solver<tPiecewise> {
      auto get_rhs() { return Vector(dpiecewise(t < 0, 3 * t * t, 2 * t)); }
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(-1, 1, ConstantStepsize(0.3),
                           std::tuple_cat(std::make_tuple(StepEvent(t | x))));

    std::cout << "tPiecewise:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct DeltaEq : Solver<DeltaEq> {
      auto get_rhs() { return delta(t) | delta(t - 1); }
      auto get_ic() { return Constant(0.) | Constant(0.); }
    } eq;
    auto sol =
        eq.solution(-1, 2, ConstantStepsize(1.1),
                    std::tuple_cat(std::make_tuple(StepEvent(t | x | y))));

    std::cout << "DeltaEq:" << std::endl;
    std::cout << sol << std::endl; //
  }

  {
    struct ReluEq : Solver<ReluEq> {
      auto get_rhs() { return Vector(2 * drelu(t)); }
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(-1, 1, ConstantStepsize(0.3),
                           std::tuple_cat(std::make_tuple(StepEvent(t | x))));

    std::cout << "ReluEq:" << std::endl;
    std::cout << sol << std::endl; //
  }
  {
    struct ClipEq : Solver<ClipEq> {
      auto get_rhs() { return Vector(dclip(t, -1, 1.)); }
      auto get_ic() { return Vector(Constant(0.)); }
    } eq;
    auto sol = eq.solution(-2, 2, ConstantStepsize(0.75),
                           std::tuple_cat(std::make_tuple(StepEvent(t | x))));

    std::cout << "ClipEq:" << std::endl;
    std::cout << sol << std::endl; //
  }
  return 0;
}
