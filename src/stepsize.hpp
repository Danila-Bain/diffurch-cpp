#pragma once
#include <algorithm>
#include <math.h>

struct ConstantStepsize {

  double initial_stepsize;

  ConstantStepsize(double ss = 0.05) : initial_stepsize(ss) {};

  template <typename RK = void>
  constexpr static bool set_stepsize(auto &state) {
    return false;
  }
};

struct AdaptiveStepsize {
  double atol = 0.001;
  double rtol = 0.001;
  double initial_stepsize = 0.05;
  double safety_factor = 4; // more is safer but more expencive
  double max_factor = 5;

  template <typename RK, typename StateT> bool set_stepsize(StateT &state) {
    static constexpr size_t n = StateT::n;

    double error = 0;
    for (size_t i = 0; i < n; i++) {
      error = max(error,
                  state.error_curr[i] / (atol + abs(state.x_curr[i]) * rtol));
    }

    error *= safety_factor; // makes the resulting step is smaller

    double fac;
    static constexpr size_t q = std::min(RK::order_embedded, RK::order);
    fac = pow(error, -1. / (q + 1.));
    fac = std::min(fac, max_factor);
    fac = std::max(fac, 1. / max_factor);

    state.t_step = state.t_step * fac;
    return error > 1;
  };
};

/*auto a = AdaptiveStepsize{.atol = 1, .rtol = 3};*/
