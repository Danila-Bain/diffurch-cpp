# Symbolic stuff

For convinence of coding, some basic symbolic classes are introduced, which (in theory) are optimized away at compile time.

Several types of symbolic expressions are defined: ```StateExpression```, ```StateBoolExpression```, and ```StateEventExpression```, which are empty classes, from which other auxilary classes are derived by means of inheritance. This inheritance is used in order to differentiate derived classs, overload math functions (i.e. ```sin```) for those equations, etc.

## State Expression

### Introduction

The types that inherit from ```StateExpression``` are expected to implement the following methods:
- ```auto operator()(const auto& state)```: evaluate the state at the current moment
- ```auto prev(const auto& state)```: evaluate the state at the previous moment
- ```auto operator()(const auto& state, double t)```: evaluate the state at the time ```t```
- ```auto operator()(double t)```: evaluate expression at the time ```t```. Not defined for expressions that contain equation variables.
- ```auto get_events()```: some expressions may introduce events; this method returns events for this expression and its subexpressions.

The basic building blocks for state expressions are the classes ```Constant<typename T>```, ```TimeVariable```, and ```Variable<size_t coordinate = -1, size_t derivative = 0>```. Let's take a look at the definition of the equation of a pendulum:

```c++
struct PendulumEquation : Solver<PendulumEquation> {

  double w;
  PendulumEquation(double w_ = 1.) : w(w_) {};

  static constexpr auto t = State::TimeVariable();
  static constexpr auto x = State::Variable<0>();
  static constexpr auto Dx = State::Variable<1>();

  auto get_rhs() { return Dx | - w * w * sin(x); }
  auto get_ic() { return std::make_tuple(0., 1.); }
};
```

Here we are focusing on the method `get_rhs`. It returns the state expression ```Dx | - w * w * sin(x)```, that through function and operator overloading, evaluates to the class, which implementation is equivalent to

```c++
struct PendulumRHS {
    std::array<double, 2> operator()(const auto& state) {
        return { state.x_curr[1], - w * w * sin(state.x_curr[0]) };
    }
    std::array<double, 2> prev(const auto& state) {
        return { state.x_prev[1], - w * w * sin(state.x_prev[0]) };
    }
    std::array<double, 2> operator()(const auto& state, double t) {
        auto x_eval = state.eval(t);
        return { x_eval[1], -w * w * sin(x_eval[0]) };
    }
}
```

with the remark, that at the time of evaluation, the value of ```w``` is known double value, and it is copied. Note that this class doesn't implement the method `operator()(double t)`, because this expression depends on the variables in the state. Also, here the method ```get_events``` is just ommited for readability. 

More specifically, the expression ```Dx | -sin(x)``` turns into 
```
State::Vector(
    State::Variable<1>(), 
    State::Mul(
        State::Constant(-w * w),
        State::Function_sin(State::Variable<0>())
    )
)
```

### TimeVariable

The class ```TimeVariable``` represents independent variable, that is usually interpreted as time.

### Variable

The class ```Variable<size_t coordinate = -1, size_t derivative = 0>``` represents the state variable or its derivative, the coordinate value of `-1` corresponds to the whole state vector.

### Constant

The class ```Constant<typename T>``` can be initialized by any value that doesn't inherit from `StateExpression`. Any non-`StateExpression` value, that touches `StateExpression` by means of arithmetical operators by means of operator overloading is converted to a `Constant`. For example, 
`State::TimeVariable t; auto expr = 2.*t + exp(t) + log(2.);`
is equivalent to 
```c++
auto expr = State::Sum(
                State::Sum(
                    State::Mul(
                        State::Constant(2.), 
                        State::TimeVariable()
                    ),
                    State::Function_exp(State::TimeVariable())
                ),
                State::Constant(log(2.))
            );
```

### VariableAt

The class `VariableAt<size_t coordinate, IsStateExpression Arg, size_t derivative = 0>` represents the state variable (if `coordinate` is `-1`, then it is the whole vector), evaluated at the other state expression. This feature is used to define delayed differential equations, for example,

```
State::TimeVariable t;
State::Variable<0> x;

auto x_delayed = x(t - 1.);
```
Here ```x_delayed``` is of type `State::VariableAt<0, State::Sub<State::TimeVaraible, State::Constant>, 0>`.

### Vector

The class ```Vector<typename...>```, that is initialized as `State::Vector(Dx, -sin(x))` represents the vector of corresponding equations. When evaluated, the result is of type `array<double, n>` with `n` matching the number of arguments in the vector.

For derived classes for `StateExpression`, `operator|` is overloaded such that vector of expressions can be written as `Dx | - sin(x)` or `sigma * (x - y) | x * (rho - z) - y | x * y - beta * z`.


### Math 

All arithmetic operators, including binary +,-,*,/, and unary +,- are overloaded.

Additionally, for derived classes for `StateExpressions`, the functions `sin`,`cos`,`tan`,`exp`,`log`,`log10`,`log2` are overloaded.


### Differentiation

For all classes the differential operator `D<size_t derivative_order = 1>` is defined, which is a function that takes some state expression, and returns a state expression, corresponding to its analytical derivative. It is defined for all state expressions mentioned above.


## StateBoolExpression

State bool expressions are just like state expressions, but they only evaluate to bool and can be used with logical operators `!`, `||`, `&&`, and `!=` for xor.

## Events (unimplemented)

For events, detection handler, save handler, and set handler are to be specified.

For detection, the syntax is this:
```
x >> 0 && y < 0
```
this corresponds to an event, that triggers when x is zero and y is negative. For events of directional zero crossing, use `x ^ 0` for crossing zero from below, and `0 ^ x` for crossing zero from above.

For save handler, the syntax is:
```
t & x & y & z
```
or
```
t | x | y | z
```

For set handler, the syntax is
```
x << -0.9*x && y << y + x
```
