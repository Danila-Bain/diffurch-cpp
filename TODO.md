The list below represents a combination of high-priority work, nice-to-have features, and random ideas. We make no guarantees that all of this work will be completed or even started. If you see something that you need or would like to have, let us know, or better yet consider submitting a PR for the feature.
# Top priority 

Here I list what, in my opinion, presents the most challenge for the progress of this project.

- Delay propagated events.
    The idea is that, for delay differential equations, we need to catch initial discontinuity, and th propagated discontinuity points. For example, equation with `x(t-1)` should include the events `t - 1 == state.t_init`, `t - 2 == state.t_init`, etc. With the current framework, it can by done by manually adding the events `EventOnce(When(t-1 == state.t_init))`, `EventOnce(When((t-1)-1 == state.t_init))`, etc.
    
    In the case, when the delay equation has events or discontinuities, delay propagated events are also required.

    The propagated events should be managed automatically, the number of propagations depends on the order of the Runge-Kutta method.

    Say, there is a discontinuity at the point `t_0`, which is found by zero crossing of function `b`, and there is a delayed term `x(t-1)`. Since `t_0` is already known, it would be wastefull to copute `t_0 + 1` as a zero of `b(t-1)`, it is prefferable to compute it as a zero of `t - 1 == t_0`. If several events are present on the unit time interval, the it is not sufficient just to save next `t_0` to track, it would be a queue, which is ugly. Or, alternatively, we can track the discontinuities by zero stepsizes. I.e., if between two step endpoints, the interval `t_prev - 1`, `t_curr - 1` contains a zero stepsize, then it is a dicontinuity and we need to step on it. The order of the discontinuity can be written in an unused `K_curr` variable, and then recovered and increased by 1.

- Inractive interface support (different ways to pipe the data).

- Error testing without analytic solution.

# Symbolic

- discontinuous functions `floor`, `ceil`, `mod`, etc.
- delta functions in the right handside, such that `x * delta(t-1)` results in a jump of a magnitude `x` at `t==1` in the corresponding variable. The delta function introduces several difficulties:
  - Delta function is more restricted than regular functions, because delta can only appear linearly in rhs coordinate, as in `a + b * delta(c)`, where `a`,`b`, and `c` are any regular expressions. This can be treated by introducing StateDeltaExpression, and restricting the rules, by which it can be combined with itself or other StateExpression's. 
  - Delta function explicitly changes the state, but the affected coordinate is not known from within. I.e. in `Vector(delta(t), 0.)`, the `delta(t)` doesn't know that it is part of the vector at the first coordinate. We can tell it, by specifying the coordinate explicitly like `Vector(delta<0>(t), 0.)`, which is not ideal, because user could unknowingly write an erroneous expression `Vector(delta<1>(t), 0.)`, which would not behave in expected way. Since the coordinate information is needed only for constructing events, the `get_events` function could accept an optional template argument, that specifies the coordinate of the expression in the vector. Such template argument then can be passed down to any subexpressions. 
- delta function can be proven to be usefull for equations with impacts, i.e., for a bouncing ball, for which the equation can be written as (x' | Dx' = ) `Dx | -g - 0.9 * delta(x) * Dx`, as well as in implementing variational equation for discontinuous systems, which is one of the initial goals of this project.

- For rhs and ic, if it is not a vector, convert it to a vector automatically, or use plain double everywhere instead.

# Events Detection

- Delay propagated events. (CHALLENGE)

- Allow for multiple event detection conditions, like in Wolfram Mathematica.
- For zero crossing events, check not only changing signs, but also changing of of the sign of the derivative. In cases, when zero crossings are alike of the function `t^2 - 0.001`, catching points where the derivative changes allows to not miss near-by sign changes of the function. Also, Wolfram Mathematica does this with ("DetectionMethod" -> "DerivativeSign").
- For derivatives of the state variable, try to evaluate it from rhs of the equation.
- Support cruder location methods, like WolframMathematica's "StepEnd", "StepBegin", and "LinearInterpolation".
- Implement Brent's root finding algorithm for event location (like in WolframMathematica)

# Special Event Types
- Modify events with Event(...).every(size_t n), such the event is triggered every n'th time.
- Modify events with Event(...).every(double t), such the triggers of the event are separated at least t in time.
- Add the event SubStepEvent(size_t n, ...), which would trigger after each step and evaluate its callbacks n times per step at an intermediate points.

# Event Saving (for plain values)

CHALLENGE:
- (implemented) write the values in a tuple of vectors
- write the values in the prescribed external vectors
- pipe the values in a file as table (usefull for a very large outputs);
- pipe the values in std stream, i.e., just print the values;
- pipe the values in a histogram (or aggregate the values in other ways);

# Special Event Saving

- save the whole interpolating function
- save one int or double value instead of a vector (for a singular events, like StartEvent, StopEvent, or ones having DisableEvent).

# Event Setting
- Add set handler StopIntegrationAfter(size_t n), which would stop integration after it is triggered n times.
- Add set handler StopIntegrationWhen(bool&), which, when triggered, would check external variable, and stop integration if it is set to true.
- Add set handler StopIntegrationWhen(StateBoolExpression), which stops integration if StateBoolExpression evaluates to true.
- Add set handlers Disable, DisableAfter, DisableWhen, etc, that disables current event (like "RemoveEvent" in Wolfram Mathematica)
- Alternatively, make special event EventOnce.

# Descrete variables
- Extend piecewise functionality to include descrete variables that change by completly arbitrary events
- Find a way to expose descrete variables for saving or external setting

# Performance

- Add performance tests, which are supposed to be run to compare versions.
- Modify the state class, such that data sequence is stored in a queue instead of vector.

# Stepsize Control

- PI vs I stepsize controllers (now only "I" is available in the form of AdaptiveStepsize)
- AdaptiveStepsize needs testing

# Testing functions

- Extend `global_error` function to equations with no knwon analytical solution.

# General ideas about testing

There is a Russian catch phrase that is translated as "everything is known by comparison", which is how I percieve the process of testing. Testing is a "do and compare" process, usually comparing the result with the expected result. By the nature of this project, obtained results are approximations of the solutions, not exact solutions. Moreover, the theory on how good those approximations should be relies on unrealistic assumption that the stepsize of the method is smalle "enough". The stepsize, of course, cannot be arbitrary small, because too many steps take too much time, and too small stepsize can event hurt the precision due rounding errors dominating the theoretical error of the method itself. Due to variety in practical implementation of methods, and to ensure the quality of the implementation, testing is required. 

Regarding the choice of test equations, obviously, equations with analytical solutions are preffered, because analytical solutions provide the reference to compare numerical solutions to. It is not hard to provide examples with simple solutions (limit fixed points, cycles, invariant torus) with different properties (stability, structural stability, etc) that are described analytically. For complex dynamics that involves chaos, it is not clear how to proceed. I do know how to produce a "chaotic" family of functions, but I don't know how to make the differntial equation that accepts it as a general solution. Hence, for testing that envolve chaotic dynamics, only better approximations can serve as reference for coarse approximations. 

Then the question is what to compare:
- Different equations
- Different Runge-Kutta schemes
- Different interpolation schemes
- Different stepsize control schemes with different parameters (i.e. initial stepsize or error tolerance)
- Different ways of approximating global error
- Other realizations of numerical methods, i.e. Wolfram Mathematica, scipy, matlab, other libraries

And what to compare by:
- Global error of solution
- Global error of interpolant
- Quality of approximation of the global error
- Error in some computed data (i.e. zero crosses)
- RHS function calls 
- Arithmetic operations count
- The other stuff

The main challenge lies not in the list of what to compare by, but in the list of what to compare. Because there, some of the points are independent of each other, and complete testing would require, for example, to test every equation with every Runge-Kutta scheme with wide range of constant stepsizes, and wide range of error tolerance for adaptive stepsize. This is huge number of tests combinatorically, and it can be hard to comprehand the results, even if we proceed with such exaustive comparison. 

What I propose is to test different things separately, because truly exaustive testing is imposible. For example, we can 
- fix Runge-Kutta method and compare how it is performing for different equations and choises of the stepsize;
- fix equation, and compare different Runge-Kutta schemes, stepsize controllers (or do it separetely for a handful of equations of different nature);

# General thoughts about testing 2

Why test? 

## User need to know that the results are reliable

It is impossible to test every equation against every method. So, it is impossible to test everything by myself and then present my library like the magic solution for any program. Instead, I should facilitate the user to test the efficiency of the chosen methods for their particular task. The function `diffurch::test::global_error` is the example: it is easy to check how the chosen method performs against the stepsize, if the analytical solution is known. 

## Library developer needs to avoid bugs

For that, a handful of test equations will suffice, which would show issues if their numerical solutions don't match the analytical solutions with sufficient precision. The main challenge here is that it is not clear how to automate the testing process. We can make a lot of tests, but with the approximating nature of the subject, I usually rely just on the visual test. We can save the result of the tests, and compare them between versions, ensuring that precision does not get worse after each update.

# General testing

- General testing of Runge-Kutta methods.
  - Testing for stability of each method, comaring with the analytical results
  - Testing the order of convergence 
  - Testing the order of interpolants

- Provide ordinary, delay, and neutral delay differential equations with analytical solutions of various types of behavior:
    - fixed points:
      - hyperbolic: various linear systems
    - unbounded solutions
    - stable and unstable hyperbolic periodic orbits
      - steady cycles, relaxation cycles (with big variation in derivative)
    - various non-hyperbolic periodic orbits
    - trajectories on hyperbolic invariant torus

# Optimization Ideas
- For `eval` method of `state`, add a template parameter, such that only the required coordinate is computed.


# Other Ideas 
- Instead of delta_x_hat, provided by the embedded scheme, we can compute the error estimation directly, if we use coefficient vector (b - bb) in place of bb.
- For now, the "solution" function is provided for equations by means of curiously recurring template pattern. I think it would be usefull to provide several interfaces, such as simply define a template function, that accepts equation class object as a parameter (which also would have to define "get_ic" and "get_rhs" methods).
- By means of symbolic differentiation, it is possible to automatically derive the variational equation, even for discontinuous equations.
- Possiblity to extending this library to work with partial differential equations is yet to be explored. Although the discretization (reducing to ODE) can be done manually, perhaps for many types of problems the discretization can be done automatically.

# Known Issues

- For delayed equations, stepsizes smaller than the delay are not handled.
- The order of output vectors corresponding to events is not the same as the order in which those events are defined

