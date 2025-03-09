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

struct ConstRHS : Solver<ConstRHS> {

  double c;
  double x0;

  ConstRHS(double c_, double x0_) : c(c_), x0(x0_) {};

  auto get_rhs() { return Vector(Constant(c)); } // x' = c
  auto get_ic() { return Vector(Constant(x0)); } // x(0) = x0
};

int main(int, char *[]) {

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.4),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }
  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.500),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }
  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.49),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }
  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.500),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t - 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.500),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t + 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.500),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t - 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.4),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }
  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t + 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol =
        eq.solution(-1, 1, ConstantStepsize(0.4),
                    std::make_tuple(Event(When(x == 0), t), StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t + 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol = eq.solution(-1, 1, ConstantStepsize(0.4),
                           std::make_tuple(Event(When(x == 0), t | 0),
                                           Event(When(x == 0), t | 1),
                                           StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  {
    struct EQ : Solver<EQ> {
      auto get_ic() { return Vector(t * (t + 2)); }
      auto get_rhs() { return D(get_ic()); }
    } eq;
    auto sol = eq.solution(-1, 1, ConstantStepsize(0.4),
                           std::make_tuple(Event(When(x == 0), t | 0),
                                           Event(When(x == 0.00001), t | 1),
                                           StepEvent(t | x)));
    std::cout << sol << std::endl; //
  }

  return 0;
}
