#include "../src/events.hpp"
#include "../src/solver.hpp"
#include "../src/symbolic.hpp"
#include "../src/util/print.hpp"
#include <iostream>
#include <matplot/matplot.h>

auto t = State::TimeVariable();
auto [x, y] = State::Variables<2>();

struct HarmonicOscillator : Solver<HarmonicOscillator> {
  double x_0 = 1.;
  double y_0 = 0.;

  auto get_lhs() { return State::Vector(y, -x); }

  static const bool ic_is_true_solution = true;

  auto get_ic() {
    return State::Vector(x_0 * cos(t) + y_0 * sin(t),
                         -x_0 * sin(t) + y_0 * cos(t));
  }

  auto get_events() {
    /*return Events(StepEvent(std::make_tuple(t, x, y)),*/
    /*              StepEvent(nullptr, [](auto &state) {*/
    /*                std::cout << state.t_curr << std::endl;*/
    /*              }));*/

    return Events(StepEvent(std::make_tuple(t, x, y)));
  }

  /*Events events(SaveSteps());*/
};

int main(int, char *[]) {

  HarmonicOscillator eq;

  eq.x_0 = 1;
  eq.y_0 = 0;

  auto true_solution = eq.get_ic();

  std::cout << true_solution(0.) << std::endl;

  std::cout << true_solution(3.141592 / 2) << std::endl;

  auto [t, x, y] = eq.solution(0., 10.);

  /*std::cout << res << std::endl;*/

  using namespace matplot;
  /*std::vector<double> x = linspace(0, 2 * pi);*/
  /*std::vector<double> y = transform(x, [](auto x) { return sin(x); });*/

  std::vector<double> true_x = transform(t, [](auto t) { return cos(t); });

  std::vector<double> diff =
      transform(x, true_x, [](auto x, auto xx) { return x - xx; });

  /*std::cout << diff << std::endl;*/
  plot(t, x, "-");
  plot(t, true_x, "-");
  /*plot(t, diff, "-");*/
  /*plot(x, y);*/
  /*hold(on);*/
  /*plot(x, transform(y, [](auto y) { return -y; }), "--xr");*/
  /*plot(x, transform(x, [](auto x) { return x / pi - 1.; }), "-:gs");*/
  /*plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");*/

  show();

  return 0;
}
