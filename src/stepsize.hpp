#pragma once

struct ConstantStepsize {
  double stepsize;

  ConstantStepsize(double ss = 0.05) : stepsize(ss) {};

  double initial_stepsize() { return stepsize; }

  template <typename... Args>
  double update_stepsize([[maybe_unused]] Args... args) {
    return stepsize;
  }

  constexpr static bool reject_step([[maybe_unused]] double error) {
    return false;
  }
};
