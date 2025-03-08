# Stepsize Control Methods

General stepsize controller is expected to have the following structure:
- **Members**:
  - `initial_stepsize` : `double`. The initial stepsize. It initializes `state.t_step` at the start of the integration.
- **Methods**:
  - `set_stepsize(auto &state) -> bool`. Adjusts the `state.t_step` based on `state.error_curr`. Returns `true` when it suggests to reject the last step, i.e. if the step needs to be redone due to excessive error, and returns `false` otherwise.

## Constant Step Size

- **Class**: `ConstantStepsize`
- **Members**:
  - `initial_stepsize` : `double`. Fixed step size used throughout the integration.
- **Methods**:
  - `set_stepsize(auto &state) -> constexpr static bool`. Always returns `false`, because steps are never rejected.

### Examples
```
auto cs1 = ConstantStepsize(0.2);
auto cs2 = ConstantStepsize(0.02);
```

## Adaptive Step Size

- **Class**: `AdaptiveStepsize`
- **Members**:
  - `atol = 1.e-7` : `double`. Absolute tolerance for error estimation.
  - `rtol = 1.e-7` : `double`. Relative tolerance for error estimation.
  - `initial_stepsize = 0.05` : `double`. Initial step size for integration.
  - `safety_factor = 4` : `double`. Factor controlling the step size adaptation; higher values prioritize stability.
  - `max_factor = 5` : `double`. Maximum allowed increase or decrease in step size.
  - `max_stepsize = 10` : `double`. Upper bound on the step size.
  - `min_stepsize = 1.e-7` : `double`. Lower bound on the step size.
- **Methods**:
  - `set_stepsize(auto &state) -> bool`. 

### Examples
```
auto as = AdaptiveStepsize{.atol=1.e-6, .rtol=1.e-6};
```
