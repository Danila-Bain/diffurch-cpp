#pragma once

#include <limits>

// Special Set Handler
struct StopIntegration {

  void operator()(auto &state) {
    state.t_prev = state.t_curr;
    state.t_curr = std::numeric_limits<double>::max();
  }
};
