#pragma once
#include <algorithm>
#include <math.h>

namespace diffurch {

struct ConstantStepsize {

  double initial_stepsize;

  ConstantStepsize(double ss = 0.05) : initial_stepsize(ss) {};

  template <typename RK = void>
  constexpr static bool set_stepsize(auto &state) {
    return false;
  }
};

struct AdaptiveStepsize {
  double atol = 1.e-7;
  double rtol = 1.e-7;
  double initial_stepsize = 0.05;
  double safety_factor = 4; // more is safer but more expencive
  double max_factor = 5;

  double max_stepsize = 10;
  double min_stepsize = 1.e-7;

  template <typename RK, typename StateT> bool set_stepsize(StateT &state) {
    static constexpr size_t n = StateT::n;

    double error = 0;
    for (size_t i = 0; i < n; i++) {
      error = std::max(error, std::abs(state.error_curr[i]) /
                                  (atol + std::abs(state.x_curr[i]) * rtol));
    }

    double fac;
    static constexpr size_t q = std::min(RK::order_embedded, RK::order);
    fac = pow(error * safety_factor, -1. / (q + 1.));
    fac = std::min(fac, max_factor);
    fac = std::max(fac, 1. / max_factor);

    state.t_step =
        std::min(max_stepsize, std::max(min_stepsize, state.t_step * fac));
    return error > 1;
  };
};

/*auto a = AdaptiveStepsize{.atol = 1, .rtol = 3};*/
} // namespace diffurch
