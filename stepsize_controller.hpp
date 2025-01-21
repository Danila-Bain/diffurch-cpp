#pragma once

/*template <typename T>*/
/*concept StepsizeControllerType = requires { false; };*/

struct ConstantStepsize {
  double stepsize;

  ConstantStepsize(double ss) : stepsize(ss) {};

  double initial_stepsize() { return stepsize; }

  template <typename... Args>
  double update_stepsize([[maybe_unused]] Args... args) {
    return stepsize;
  }

  constexpr static bool reject_step([[maybe_unused]] double error) {
    return false;
  }
};
