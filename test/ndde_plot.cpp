#include "../src/equations.hpp"
#include "../src/rk_tables/rktp64.hpp"
#include "../src/test/global_error.hpp"
#include "../src/util/print.hpp"
/*#include <iostream>*/
/*#include <cmath>*/
/*#include <matplot/matplot.h>*/

#include <matplotlibcpp.h>
#include <numpy/arrayobject.h>

namespace plt = matplotlibcpp;

namespace State {
static constexpr auto t = TimeVariable();
static constexpr auto x = Variable<0>();
static constexpr auto Dx = Variable<0, 1>();

} // namespace State

int main(int, char *[]) {

  {
    auto eq = Equation::LinearNDDE1Sin(1.);

    auto [t, x, xtau, Dxtau] = eq.solution<rktp64>(
        0, 10, ConstantStepsize(0.1),
        Events(StepEvent(SaveAll<decltype(eq)>()),
               StepEvent(State::x(State::t - 1.) & State::Dx(State::t - 1.))));

    auto ic = eq.get_ic();

    vector<double> x_true(t.size());
    std::transform(t.begin(), t.end(), x_true.begin(),
                   [&ic](double t_) { return ic(t_)[0]; });

    /*auto err = global_error<rk98>(eq, 0, 10, stepsizes);*/
    /*std::cout << err << std::endl;*/
    plt::plot(t, x);
    plt::plot(t, x_true);
    plt::plot(t, xtau);
    plt::plot(t, Dxtau);
  }

  /*plt::ylim(1.e-17, 1.e-5);*/
  plt::legend();
  plt::tight_layout();
  plt::show();

  return 0;
}
