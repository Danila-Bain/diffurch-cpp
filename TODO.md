The list below represents a combination of high-priority work, nice-to-have features, and random ideas. We make no guarantees that all of this work will be completed or even started. If you see something that you need or would like to have, let us know, or better yet consider submitting a PR for the feature.

# Symbolic

- non-smooth functions, including `abs`, `clip<0,1>`, etc.
- discontinuous functions, including `sign`, `Heaviside` (or, better named, `step`), `floor`, and, most importantly, `piecewise`.
- delta functions in the right handside, such that `x * delta(t-1)` results in a jump of a magnitude `x` at `t==1` in the corresponding variable.


# IO

- different output policies, including 1) (current) tuple of vectors, 2) .dat files (space separated columns), 3) .csv file


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
