#pragma once

/*template <typename T>*/
/*concept StepsizeControllerType = requires { false; };*/

struct ConstantStepsize {
  double stepsize;

  double initial_stepsize() { return stepsize; }

  template <typename... Args> double update_stepsize(Args... args) {
    return stepsize;
  }

  constexpr static bool reject_step(double error) { return false; }
};
