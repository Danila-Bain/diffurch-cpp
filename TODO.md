The list below represents a combination of high-priority work, nice-to-have features, and random ideas. We make no guarantees that all of this work will be completed or even started. If you see something that you need or would like to have, let us know, or better yet consider submitting a PR for the feature.

# Symbolic

- non-smooth functions `clip`, `relu`, etc.
- discontinuous functions `floor`, `ceil`, `mod`, etc.
- delta functions in the right handside, such that `x * delta(t-1)` results in a jump of a magnitude `x` at `t==1` in the corresponding variable. The delta function introduces several difficulties:
  - Delta function is more restricted than regular functions, because delta can only appear linearly in rhs coordinate, as in `a + b * delta(c)`, where `a`,`b`, and `c` are any regular expressions. This can be treated by introducing StateDeltaExpression, and restricting the rules, by which it can be combined with itself or other StateExpression's. 
  - Delta function explicitly changes the state, but the affected coordinate is not known from within. I.e. in `Vector(delta(t), 0.)`, the `delta(t)` doesn't know that it is part of the vector at the first coordinate. We can tell it, by specifying the coordinate explicitly like `Vector(delta<0>(t), 0.)`, which is not ideal, because user could unknowingly write an erroneous expression `Vector(delta<1>(t), 0.)`, which would not behave in expected way. Since the coordinate information is needed only for constructing events, the `get_events` function could accept an optional template argument, that specifies the coordinate of the expression in the vector. Such template argument then can be passed down to any subexpressions. 

# Events Detection

- Test simultanious events handling

# Event Types

- Modify events with Event(...).every(size_t n), such the event is triggered every n'th time.
- Modify events with Event(...).every(double t), such the triggers of the event are separated at least t in time.
- Add the event SubStepEvent(size_t n, ...), which would trigger after each step and evaluate its callbacks n times per step at an intermediate points.

# Event Saving
- (implemented) write the values in a tuple of vectors
- pipe the values in a file as table (usefull for a very large outputs).
- pipe the values in a histogram (or aggregate the values in other ways)

# Performance

- Add performance tests, which are supposed to be run to compare versions.
- Modify the state class, such that data sequence is stored in a queue instead of vector.

# Stepsize Control

- PI vs I stepsize controllers (now only "I" is available in the form of AdaptiveStepsize)
- AdaptiveStepsize needs testing

# Other Ideas 

- For now, the "solution" function is provided for equations by means of curiously recurring template pattern. I think it would be usefull to provide several interfaces, such as simply define a template function, that accepts equation class object as a parameter (which also would have to define "get_ic" and "get_rhs" methods).

# Known Issues

- For delayed equations, if stepsize is smaller than the delay, it is not handled and results in program termination.
- When there is an event, which alters the state, and it makes zero step. This zero step does not trigger other step events.
