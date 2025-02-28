#pragma once

#include "../solver.hpp"
#include <algorithm>

#include "../util/print.hpp"
#include <iostream>
#include <tuple>

namespace diffurch::test {

template <typename RK = rk98, typename StepsizeController>
std::vector<double>
global_error(auto equation, double initial_time, double final_time,
             const std::vector<StepsizeController> &stepsize_controllers) {

  static_assert(
      requires { equation.ic_is_true_solution; } &&
          equation.ic_is_true_solution,
      "Equation has to have member ic_is_true_solution set to compare numeric "
      "solution to true solution.");

  auto true_solution = equation.get_ic();
  static constexpr size_t n =
      std::tuple_size<decltype(true_solution(0.))>::value;

  std::array<double, n> true_x = true_solution(final_time);
  /*cout << true_solution(t) << endl;*/
  std::vector<double> error(stepsize_controllers.size());
  std::transform(
      stepsize_controllers.begin(), stepsize_controllers.end(), error.begin(),
      [&](StepsizeController stepsize_controller) {
        auto res = equation.template solution<RK>(
            initial_time, final_time, stepsize_controller,
            std::make_tuple(StopEvent(SaveAll<decltype(equation)>())));
        double t;
        std::array<double, n> x;
        std::apply(
            [&](auto t_, auto... x_i) {
              t = t_[0];
              x = std::array<double, sizeof...(x_i)>({x_i[0]...});
            },
            res);

        // std::cout << res << std::endl;
        // std::cout << stepsize_controller.initial_stepsize << std::endl;
        return norm(x - true_x) / (1 + norm(true_x));
      });
  return error;
}

template <typename RK = rk98>
std::vector<double> global_error(auto equation, double initial_time,
                                 double final_time,
                                 const std::vector<double> &stepsizes) {

  std::vector<ConstantStepsize> stepsize_controllers(stepsizes.size());
  std::transform(stepsizes.begin(), stepsizes.end(),
                 stepsize_controllers.begin(),
                 [](double stepsize) { return ConstantStepsize(stepsize); });
  return global_error<RK>(equation, initial_time, final_time,
                          stepsize_controllers);
}

} // namespace diffurch::test
