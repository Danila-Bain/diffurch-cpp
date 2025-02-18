The list below represents a combination of high-priority work, nice-to-have features, and random ideas. We make no guarantees that all of this work will be completed or even started. If you see something that you need or would like to have, let us know, or better yet consider submitting a PR for the feature.

# Symbolic

- non-smooth functions, including `abs`, `clip<0,1>`, etc.
- discontinuous functions, including `sign`, `Heaviside` (or, better named, `step`), `floor`, and, most importantly, `piecewise`.
- delta functions in the right handside, such that `x * delta(t-1)` results in a jump of a magnitude `x` at `t==1` in the corresponding variable.
