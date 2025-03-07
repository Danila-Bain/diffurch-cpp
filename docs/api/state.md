# State

``State`` is the container template class that stores current, previous, and past states of the equation during integration. It is modified during the application of each Runge-Kutta step. Also, `State` keeps the sequence of past states that are used in delay differential equations, and it supports the interpolation for those past states, provided by dense output Runge-Kutta schemes (for that, calculated Runge-Kutta stages are saved as well).

## Details 

Class signature:
```c++
template <typename RK, typename InitialConditionHandlerType> struct State;
```
where
- `RK` is the class that holds used Runge-Kutta scheme. Dense output RK schemes are used for interpolation of past steps. See [Runge Kutta Table classes](rk_tables.md).
- `InitialConditionHandlerType` is the type of an object that defines `operator()(double initial_time) -> std::array<double, n>`, which determines the initial condition (the state is initialized by calling this operator at the initial_time), and, importantly, the dimensiality of the state (the number `n`, the size of an array, holding the current, previous, or past states).

Class members:
- independent variable:
    - `t_curr` : `double`
    - `t_prev` : `double`
    - `t_step` : `double`
    - `t_sequence` : `std::vector<double>`
- dependent variable:
    - `x_curr` : `std::array<double, n>`
    - `x_prev` : `std::array<double, n>`
    - `x_sequence` : `std::vector<decltype(x_curr)>`
    - `error_curr` : `std::array<double, n>`
- Runge-Kutta stages:
    - `K_curr` : `std::array<decltype(x_curr), RK::s>` 
    - `K_sequence` : `std::vector<decltype(K_curr)>` 
   

Class methods:
- constructor: `State(double initial_time, InitialConditionHandlerType ic_)`
- `push_back_curr() -> void`
- `make_zero_step() -> void`
- `eval<size_t derivative_order = 0>(double t) -> decltype(x_curr)` 
