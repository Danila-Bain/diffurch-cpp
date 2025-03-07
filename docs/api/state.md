# State

``State`` is the container template class that stores current, previous, and past states of the equation during integration. It is modified during the application of each Runge-Kutta step. Also, `State` keeps the sequence of past states that are used in delay differential equations, and it supports the interpolation for those past states, provided by dense output Runge-Kutta schemes (for that, calculated Runge-Kutta stages are saved as well).

Overall, everything is dancing around state class, because it takes the role of the container of global variables during integration:
- In [solver](solver.md) main loop, the `State` object is created and Runge-Kutta stages are being calculated, written, and read in `K_curr` member variable of the `State` object. Also, the right hand side of the differential equation accepts the `State` object by constant reference. This way, it is easier to include many types of eqations within the same typing structure: autonomous sytems would just use `x_curr` member variable as its argument, non autonomuous sytems would also use `t_curr`, the systems with delay would use the member function `eval` to evaluate the solution at the past times. Otherwise, in each case it would be necessary to pass different number of parameters to the right-hand side function, or pass all at once.

- In all [events](event.md), the `State` object is passed as const reference to [event detection handler](state_detect_expression.md), which in turn can make use of `x_curr`, `x_prev` member variables to detect events, and `eval` member function to locate the precise event point. Also, [event saving handler](save_handler.md) takes const reference to the `State` object, from which it extracts the needed information to save. Finally, in [event setting handler](set_handler.md), the `State` object is either passed by a const reference or by a non-const reference, where it can be modified or just used in external callbacks.

## Details 

Class signature:
```c++
template <typename RK, typename InitialConditionHandlerType> struct State;
```
where
- `RK` is the class that holds used Runge-Kutta scheme. Dense output RK schemes are used for interpolation of past steps. If chosen RK scheme doesn't support dense output, then usage of the `eval` method will raise compiler error. See [Runge Kutta Table classes](rk_tables.md).
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
