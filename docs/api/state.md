
# State

`State` is the container template class that stores current, previous, and past states of the equation during integration. It is modified during the application of each Runge-Kutta step. Additionally, `State` maintains the sequence of past states used in delay differential equations and supports interpolation of those past states, which is facilitated by dense output Runge-Kutta schemes. To enable interpolation, previously computed Runge-Kutta stages are also stored.

Overall, the `State` class plays a central role in integration, effectively serving as a container for global variables:

- In the [solver](solver.md) main loop, a `State` object is created, and Runge-Kutta stages are calculated, stored, and accessed through the `K_curr` member variable. The right-hand side function of the differential equation accepts a constant reference to the `State` object, simplifying type handling across various equation types:

  - Autonomous systems rely on the member `x_curr` as their state variable.
  - Non-autonomous systems require both `x_curr` and `t_curr`.
  - Delay differential equations use the `eval` method to access past states dynamically.

  Without this unified structure, different equation types would require a variable number of parameters in function calls, significantly complicating the interface. On the other hand, defining the right-hand side function in terms of `State`'s member variables can be tedious as well. To tackle this, [symbolic](symbolic.md) facilities are provided, which make the definition of equations as simple and intuitive as possible.

- In all [events](event.md), the `State` object is passed as a constant reference to the [event detection handler](state_detect_expression.md). The detection handler utilizes `x_curr` and `x_prev` to identify events, while the `eval` function allows precise event location. Similarly, the [event saving handler](save_handler.md) extracts necessary information from `State` for logging, and the [event setting handler](set_handler.md) receives `State` either as a constant reference (for read-only operations) or as a non-constant reference (when modifications are needed).

## Details

### Class Signature

```c++
template <typename RK, typename InitialConditionHandlerType> struct State;
```

Here:

- `RK` is the class representing the chosen Runge-Kutta scheme. If the scheme supports dense output, past steps can be interpolated. Otherwise, attempting to use `eval` will result in a compilation error. See [Runge-Kutta Table classes](rk_tables.md) for details on the expected interface.
- `ICType` is an object type that provides an `operator()(double t) -> std::array<double, n>`, which is used to initialize the state at `t_init` and defines the dimensionality `n` of the state vector (`n` is deduced by the return type).

### Class Members

#### Independent Variables

- `t_init` : `double`. The initial time, at which the integration begins.
- `t_curr` : `double`. The current time in the integration process. During step advancing, it holds the time of the current Runge-Kutta stage. After step is done, it is the current time of the calculated step.
- `t_prev` : `double`. The time of the previous step.
- `t_step` : `double`. The step size used in current (or next) step.
- `t_sequence` : `std::vector<double>`. A sequence storing time of each step.

#### Dependent Variables

- `x_init` : `ICType`. The initial condition handler, that defines `operator()(double t) -> std::array<double, n>`. It is used to initialize the state. And it is called, when the method `eval` accepts time that is less than `t_init`.
- `x_curr` : `std::array<double, n>`. The current state of the system at `t_curr`. During step advancing, it holds the state at which the right-hand side function is evaluated for each Runge-Kutta stage. After step is done, it holds the current state value.
- `x_prev` : `std::array<double, n>`. The system state at `t_prev`, the previous system state.
- `x_sequence` : `std::vector<decltype(x_curr)>`. A history of past states, enabling delay equations.
- `error_curr` : `std::array<double, n>`. Stores error estimates for adaptive step-size control.

#### Runge-Kutta Stages

- `K_curr` : `std::array<decltype(x_curr), RK::s>`. Stores intermediate stages of the Runge-Kutta computation for the current step.
- `K_sequence` : `std::vector<decltype(K_curr)>`. A history of past Runge-Kutta stage evaluations, required for dense output interpolation.

### Class Methods

- **Constructor**: `State(double t_init, ICType x_init)`. Initializes the state at `t_init` using the provided initial condition handler.

- `push_back_curr() -> void`. Saves the current state, current time, and current Runge-Kutta stage evaluations (`x_curr`, `t_curr`, and `K_curr`, respectively) into `x_sequence`, `t_sequence`, and `K_sequence`, respectively. 

- `make_zero_step() -> void`. Performes the zero-length step, by overwriting `x_prev` and `t_prev` with `x_curr` and `t_curr` values, respectively; setting `K_curr` with zeros; and calling `push_back_curr()`. It is used when an event changes the state at the point of this call, such that this change is represented by the step of zero length. This way, interpolation quality is not affected by such abrupt change. 
- `eval<size_t derivative_order = 0>(double t) -> decltype(x_curr)`. Evaluates the state (or its derivative) at an arbitrary past time `t` using interpolation (if dense output is available). The template parameter `derivative_order`, which is zero by default, specifies the derivative order, with zero derivative order corresponding to just the state itself. If `t > t_curr`, runtime error will occur. If `t < t_init`, then `x_init` is used: when `derivative_order`=0, `x_init(t)` is returned; for `derivative_order`>0, if `x_init` is [`StateExpression`](state_expression.md), then `D<derivative_order>(x_init)(t)` is returned, else, `x_init.template eval<derivative_order>(t)` is returned.
 Additionally, if `t` between `t_prev` and `t_curr`, then only the variables `t_prev`, `t_curr`, `x_prev`, `x_curr`, and `K_curr` are used for calculation, and sequences `t_sequence`, `x_sequence`, and `K_sequence` are not used.

