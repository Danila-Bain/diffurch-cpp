#pragma once

#include "../solver.hpp"
#include <algorithm>

template <typename RK = rk98>
auto GlobalErrorVsConstantStepsize(auto equation, double initial_time,
                                   double final_time,
                                   std::vector<double> stepsizes) {

  std::vector<double> stepsize_controllers(stepsizes.size());
  std::transform(stepsizes.begin(), stepsizes.end(),
                 stepsize_controllers.begin(),
                 [](double stepsize) { return ConstantStepsize(stepsize); });
  return GlobalErrorVsConstantStepsize<RK>(equation, initial_time, final_time,
                                           stepsize_controllers);
}

template <typename RK = rk98, typename StepsizeController>
auto GlobalErrorVsConstantStepsize(
    auto equation, double initial_time, double final_time,
    std::vector<StepsizeController> stepsize_controllers) {

  static_assert(
      requires { equation.ic_is_true_solution; } &&
          equation.ic_is_true_solution,
      "Equation has to have member ic_is_true_solution set to compare numeric "
      "solution to true solution.");

  auto true_solution = equation.get_ic();
  static constexpr size_t n =
      std::tuple_size<decltype(true_solution(0.))>::value;

  std::vector<double> error(stepsize_controllers.size());
  std::transform(stepsize_controllers.begin(), stepsize_controllers.end(),
                 error.begin(), [&](StepsizeController stepsize_controller) {
                   auto res = equation.template solution<RK>(
                       initial_time, final_time, stepsize_controller,
                       Events(StopEvent(SaveAll<decltype(equation)>())));
                   double t;
                   std::array<double, n> x;

                   std::apply(
                       [&](auto t_, auto... x_i) {
                         t = t_[0];
                         x = std::array({x_i[0]...});
                       },
                       res);

                   auto true_x = true_solution(t);
                   return norm(x - true_x);
                 });
  return error;
}
