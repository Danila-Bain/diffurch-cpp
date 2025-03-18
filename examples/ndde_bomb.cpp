#include "../diffurch.hpp"
#include "../src/util/print.hpp"
#include <iostream>
#include <math.h>

#include <matplotlibcpp.h> // for plotting
#include <tuple>

namespace plt = matplotlibcpp;
using namespace diffurch;

int main() {
  struct Eq : Solver<Eq> {
    double epsilon = 0.1;
    double A = -0.1;
    double amp;
    double freq;

    Eq(double e, double a, double am, double f)
        : epsilon(e), A(a), amp(am), freq(f) {}

    auto get_rhs() {
      using namespace diffurch::variables_x_t;
      return Vector(-x + (1 + epsilon) * D(x)(t - 1) + A * pow(D(x)(t - 1), 3));
    }

    auto get_ic() {
      using namespace diffurch::variables_x_t;
      return Vector(amp * sin(freq * 2 * M_PI * t));
    }
  };

  double finish = 2000.;
  size_t last_samples = 100;

  {
    using namespace diffurch::variables_x_t;
    auto res = Eq(0.1, -0.1, 0.17, 1.)
                   .solution(0, finish, ConstantStepsize(1. / 50.),
                             std::make_tuple(StepEvent(t | x)));
    auto [t, x] = res;
    // plt::plot(t, x);
    plt::plot(std::vector(t.end() - last_samples, t.end()),
              std::vector(x.end() - last_samples, x.end()));
  }

  {
    using namespace diffurch::variables_x_t;
    auto res = Eq(0.1, -0.1, 0.085, 2)
                   .solution(0, finish, ConstantStepsize(1. / 50.),
                             std::make_tuple(StepEvent(t | x)));
    auto [t, x] = res;
    // plt::plot(t, x);
    plt::plot(std::vector(t.end() - last_samples, t.end()),
              std::vector(x.end() - last_samples, x.end()));
  }

  // plt::xlim(finish - 10, finish);

  plt::show();
}
