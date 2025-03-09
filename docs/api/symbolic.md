# Symbolic manipulation

In this project, basic symbolic functionality is implemented, allowing for a straightforward definition of the right-hand side function of a differential equation and more. There are several types of symbols with different functionalities and ways to interact with each other:

- `Symbol`: Used for functions on the state (i.e., for defining the right-hand side). It supports arithmetic operators and many standard mathematical functions. It can usually be differentiated.
- `BoolSymbol`: Used for any kind of conditions, mostly for events. It is very similar to `Symbol`, but its return type is always `bool`, and `BoolSymbol`s support boolean operators instead of arithmetic ones.
- `DetectSymbol`: Used to define an event detection handler, which supports `detect` and `locate` methods that act on the `State` object.
- `SetSymbol`: Used in event callbacks for setting state variables.

# Symbol

The library defines the empty class `Symbol` together with the pair of concepts `IsSymbol<T>` and `IsNotSymbol<T>`, which check whether the type `T` inherits from the type `Symbol`. From now on, by a "symbol," we understand a class (or an object of such a class) that inherits from the type `Symbol`. This inheritance is necessary to enable overloads of arithmetic operators and mathematical functions. The expected interface of a symbol can vary depending on the use, so it would be impractical (if even possible) to distinguish symbols from other classes based solely on the implemented methods.

A symbol can implement the following set of member functions:

- `auto operator()(const auto &state) const`: Evaluates the symbol at the current state. Typically, this method uses `state.t_curr` and `state.x_curr` members.
- `auto prev(const auto &state) const`: Evaluates the symbol at the previous state. Typically, this method uses `state.t_prev` and `state.x_prev` members.
- `auto operator()(const auto &state, double t) const`: Evaluates the symbol with the state at time `t`. Typically, this method uses the `state.eval` method.
- `auto operator()(double t) const`: Evaluates the symbol at time `t`. This is only available for symbols that do not depend on the state, such as symbols representing functions of time. This method is typically used for symbols that define the initial conditions.
- `template <size_t coordinate = -1> auto get_events()`: Retrieves any events introduced by the symbol. For example, using the symbol `dsign` will introduce a zero-crossing event for its argument. The template parameter `coordinate` signifies that the current symbol is in a particular coordinate of the `Vector`, which is useful for the `delta` function that modifies the corresponding coordinate when triggered.

Additionally, the function `template <size_t derivative_order = 1> D(const MySymbol&)` can be overloaded to define the derivative of `MySymbol`.

## Example: Variables and Constants

The basic building blocks for symbols are the classes `Constant<typename T>`, `TimeVariable`, and `Variable<size_t coordinate = -1, size_t derivative = 0>`.

Let's take a look at an example that defines the harmonic oscillator equation:

```c++
struct Oscillator : Solver<Oscillator> {

  double w;
  Oscillator(double w_ = 1.) : w(w_) {};

  static constexpr auto t = diffurch::TimeVariable();
  static constexpr auto x = diffurch::Variable<0>();
  static constexpr auto Dx = diffurch::Variable<1>();

  auto get_ic() { return sin(w * t) | w * cos(w * t); }
  auto get_rhs() { return Dx | - w * w * sin(x); }
};

Oscillator eq(2.);
auto ic = eq.get_ic();
auto rhs = eq.get_rhs();
```

Breakdown:

- The inheritance from `Solver<Oscillator>` is not relevant to the discussion of symbols here. It simply adds the `solution` methods to this class.
- The member `t` is a `TimeVariable` symbol, which represents the time variable in the state:

| Usage of `t` | Equivalent Expression |
| ------------ | --------------------- |
| `t(state)`   | `state.t_curr`         |
| `t.prev(state)`   | `state.t_prev`         |
| `t(state, 42.)` or `t(42.)`   | `42.`         |

- The method `get_ic` returns a symbol formed from other symbols:

| Usage of `ic` | Equivalent Expression |
| ------------- | --------------------- |
| `ic(state)`   | `std::array<double, 2>{sin(2. * state.t_curr), 2. * cos(2. * state.t_curr)}` |
| `ic.prev(state)`   | `std::array<double, 2>{sin(2. * state.t_prev), 2. * cos(2. * state.t_prev)}` |
| `ic(42.)` or `ic(state, 42.)`    | `std::array<double, 2>{sin(2. * 42.), 2. * cos(2. * 42.)}` |

  - The operator `|` combines the symbols into one `Vector`, which evaluates its subexpressions as the coordinates of a `std::array`.
  - The operator `*` in `w * t` is overloaded so that the result is a symbol corresponding to the time variable multiplied by `w`. When constructing the symbol for `w * t`, `w` is replaced by the symbol `Constant(w)`, which takes `w` as a copy. Note that in the column "Equivalent Expression," we use `2.` instead of `w` to emphasize that in the symbol `w * t`, the value of `w` is saved by copy. Storing `w` by reference is not supported, and currently, there is no motivation to implement it that way.

- The members `x` and `Dx` represent the coordinates of the state:

| Usage of `x` | Equivalent Expression |
| ------------ | --------------------- |
| `x(state)`   | `state.x_curr[0]`         |
| `x.prev(state)`   | `state.x_prev[0]`         |
| `x(42.)` | Not defined    |
| `x(state, 42.)` | `state.eval(42.)[0]`  |

  - For `Dx`, the equivalent expressions differ only by the use of index `[1]`.

- The `rhs` symbol behaves as follows:

| Usage of `rhs` | Equivalent Expression |
| -------------- | --------------------- |
| `rhs(state)`     | `std::array<double, 2>{ state.x_curr[1], - 2. * 2. * sin(state.x_curr[0]) }`      |
| `rhs.prev(state)`     | `std::array<double, 2>{ state.x_prev[1], - 2. * 2. * sin(state.x_prev[0]) }`      |
| `rhs(42.)` | Not defined    |
| `rhs(state, 42.)`     | `std::array<double, 2>{ state.eval(42.)[1], - 2. * 2. * sin(state.eval(42.)[0]) }`      |

  - Note that `eval` is called twice for `rhs(state, 42.)`. It is unclear whether the compiler optimizes this redundancy, but improving this aspect of the design could be beneficial.


## Classes and Functions

### `Constant<T>`
Represents a constant value that does not depend on the system state or time.

#### Template Parameters
- `T : typename`. The type of the constant value.

#### Members
- `value : T`. Stores the constant value.

#### Constructors
- `Constant(T val)`. Initializes the constant with the given value.

#### Operators
- `operator T() const`. Implicit conversion to `T`.
- All the following methods, return the value of member `value`:
    - `T operator()(const auto &state) const`
    - `T prev(const auto &state) const`
    - `T operator()(const auto &state, double t) const`
    - `T operator()(double t) const`
- `static auto get_events<size_t coordinate = -1>()` - Returns an empty tuple.

#### Differentiation
`template <typename T, size_t derivative_order = 1> Constant<T> D(Constant<T> c)`:
- If `derivative_order > 0`, returns `Constant<T>(0)`.
- Otherwise, returns `c`.

#### Notes
- If an arithmetical operator is used for a symbol and a non-symbol. The non-symbol is replaced by the corresponding `Constant`

#### Example
```c++
TimeVariable t;
auto k = Constant(3.);
auto expr1 = k * t;
auto expr2 = 3. * t; // 3. is replaced with Constant(3.)
assert(expr1(2.) == 6.);
assert(expr2(2.) == 6.);
```

### `TimeVariable`
Represents the time variable in a differential equation.

#### Methods
- `static auto operator()(const auto &state)`. Returns `state.t_curr`.
- `static auto prev(const auto &state)`. Returns `state.t_prev`.
- `static auto operator()(const auto &state, double t)`. Returns `t`.
- `static auto operator()(double t)`. Returns `t`.
- `static auto get_events<size_t coordinate = -1>()`. Returns an empty tuple.

#### Differentiation
`template <size_t derivative_order = 1> auto D(TimeVariable t)`:
- If `derivative_order == 0`, returns `TimeVariable{}`.
- If `derivative_order == 1`, returns `Constant(1.)`.
- Otherwise, returns `Constant(0.)`.


### `VariableAt<coordinate, Arg, derivative>`
Represents a dependent variable (or its derivative) at a given state.

#### Template Parameters
- `coordinate : size_t`. The coordinate index. The value `-1` corresponds to the whole state vector.
- `Arg : IsSymbol`. The argument determining the state.
- `derivative : size_t` (default: `0`). The derivative order.

#### Members
- `Arg arg`. The argument associated with the variable.

#### Methods
- `auto operator()(const auto &state) const`. Returns something like `state.eval<derivative>(arg(state))[coordinate]`.
- `auto prev(const auto &state) const`. The same but uses `arg.prev(state)`.
- `auto operator()(const auto &state, double t) const`. The same but uses `arg(state, t)`.
- `auto get_events<size_t current_coordinate = -1>()`. Returns `arg.template get_events<current_coordinate>()`.

#### Differentiation
`D(VariableAt<...>)` computes the derivative of `VariableAt` recursively applying the chain rule for the higher order derivatives.

### `Variable<coordinate, derivative>`
Represents a system state variable.

#### Template Parameters
- `coordinate` (default: `-1`). The coordinate index. The value `-1` corresponds the the whole state vector.
- `derivative` (default: `0`). The derivative order. If `derivative` is not `0`, then `state.eval` is used in all methods.

#### Members

- `static auto operator()(const IsSymbol auto& arg)` - Constructs a `VariableAt` from an argument.
- `static auto get_events<size_t coordinate = -1>()`. Returns an empty tuple.

Only if `derivative` is `0`:
- `static auto operator()(const IsNotSymbol auto &state)`. Returns `state.x_curr[coordinate]` or `state.x_curr` if `coordinate` is `-1`.
- `static auto prev(const IsNotSymbol auto &state)`. Returns `state.x_prev[coordinate]` or `state.x_prev` if `coordinate` is `-1`
- `static auto operator()(const IsNotSymbol auto &state, double t)`. Returns `state.eval(t)[coordinate]` or `state.eval(t)` if coordinate is `-1`.

#### Differentiation 
`D(Variable<...>)` returns `Variable` with an increased derivative order.


### `Variables<N, from, to, derivative_order>`
Generates a tuple of variables.

#### Template Parameters
- `N` : `size_t`. The total number of variables.
- `from` : `size_t` (default: `0`). The starting index.
- `to` : `size_t` (default: `N`). The stopping index (non-inclusive).
- `derivative_order` : `size_t` (default: `0`). The derivative order for each variable.

#### Returns
- A tuple of `Variable` instances with coordinates from `from` to `to - 1`.

#### Example
```c++
auto [x,y,z] = Variables<3>{};
```
which is equivalent to
```c++
auto x = Variable<0>{}
```


