
# Runge-Kutta Tables

## Introduction

Runge-Kutta methods constitute a broad family of numerical techniques for solving differential equations. For a general definition, refer to Wikipedia. Here, we present some fundamental concepts that may not be immediately obvious when first encountering these methods.

The core idea is to identify an optimal method within a particular class of formulas that contain numerous parameters. These parameters are determined such that the Taylor series expansion of the approximate solution (as a function of the step size) matches the Taylor series expansion of the exact solution as closely as possible. If these expansions coincide up to the $n$th power of the step size $h$, then the method is said to be of order $n$, meaning the local error per step is of order $O(h^{n+1})$, and the global error of the solution is of order $O(h^n)$. Basically, all Runge-Kutta methods differ by their size (the number of stages), achieved precision (the order), numerical stability (for which complex $\lambda$, the numerical solutions to $\dot x = \lambda x$ do not diverge), and additional features, such as automatic step size control (achieved by approximating the local error), or dense output (good interpolation of the solution between&#x20;

adjacent steps of the method).

Each Runge-Kutta method consists of several stages, denoted by $s$. For a method of order $n$, the number of required stages is roughly proportional to $s$ for small $n$ and increases slightly faster than linearly as $n$ grows. Each step requires exactly $s$ evaluations of the right-hand side function and approximately $s^2$ arithmetic operations (additions and multiplications). Consequently, as the number of stages increases, the computational effort per step grows quadratically, while the error per step decreases exponentially. This trade-off makes higher-order methods preferable both in terms of performance (since reducing the number of required steps outweighs the increased computational cost per step) and accuracy (as fewer steps result in reduced accumulation of rounding errors). For example, achieving machine precision using the Euler method is practically impossible.

Runge-Kutta methods encompass all numerical schemes that satisfy the following properties:

1. **One-step methods:** These methods compute the next approximation using only the immediately preceding approximation, as opposed to multi-step methods, which rely on multiple past values.
2. **Affine covariance (it is not an established term):** If an affine transformation (a linear transformation combined with a translation) is applied to  the dependent or independent variables in the differential equation, the numerical approximation undergoes the exact same transformation. This ensures that the method behaves consistently under rescaling, rotation, or time shifts.

The first property is particularly advantageous when handling events, discontinuities, or delay differential equations—key applications of this project. The second property serves as a basic consistency check, ensuring the method behaves predictably under transformations.

Each Runge-Kutta method is defined by its Butcher tableau, with two optional extensions:

1. **Embedded schemes** provide two sets of `b` coefficients: one for the actual approximation and another for a slightly less accurate approximation. The difference between these approximations estimates the local error, allowing for adaptive step-size selection. When the solution changes rapidly, the step size is reduced to capture finer details, whereas when the solution varies slowly, larger steps are taken without significant loss of accuracy.
2. **Dense output schemes** include polynomials that interpolate the solution between step endpoints. This interpolation is useful for event detection, visualization, and solving delay differential equations, where intermediate values are essential for maintaining accuracy.

Methods with dense output are also called **continuous Runge-Kutta methods**, in contrast to **discrete Runge-Kutta methods**, which only provide values at discrete time points.

## Runge-Kutta Table Classes

This project represents Runge-Kutta tables as classes with the following structure:

```c++
struct my_rk_table {
    // Number of stages in the method
    constexpr static size_t s;

    // Order of the method
    constexpr static size_t order;

    // (Optional) Order of the embedded scheme
    constexpr static size_t order_embedded;

    // (Optional) Order of the dense output (interpolation) formula
    constexpr static size_t order_interpolation;

    // Coefficients matrix (a)
    constexpr static std::array<std::array<double, s-1>, s> a;

    // Column vector (c)
    constexpr static std::array<double, s> c;

    // Weights (b coefficients)
    constexpr static std::array<double, s> b;

    // (Optional) Embedded weights (b coefficients for the embedded method)
    constexpr static std::array<double, s> bb;

    // (Optional) Polynomials for dense output interpolation
    constexpr static std::array<Polynomial<degree>, s> bs;
};
```

All non-optional fields are required, and the `static` keyword is essential for each field. The coefficients are not initialized in this example, but in real example they must be (like any static variables).

### Example: Classical 4th-Order Runge-Kutta Method

The coefficients for classic 4th-order Runge-Kutta method with a 3rd-order interpolation scheme is defined as follows:

```c++
struct rk4 {
    constexpr static size_t s = 4;
    constexpr static size_t order = 4;
    constexpr static size_t order_interpolation = 3;

    constexpr static std::array<std::array<double, 3>, 4> a{
        {{}, {0.5}, {0.0, 0.5}, {0.0, 0.0, 1.0}}
    };

    constexpr static std::array<double, 4> c {{0.0, 0.5, 0.5, 1.0}};

    constexpr static std::array<double, 4> b {{1./6., 1./3., 1./3., 1./6.}};

    constexpr static std::array<Polynomial<3>, 4> bs{
        {{0., 1., -1.5, 2./3.},
         {0., 0., 1., -2./3.},
         {0., 0., 1., -2./3.},
         {0., 0., -0.5, 2./3.}}};
};
```

Note here, that unspecified values are automatically initialized to 0. By defining such a class, users can implement custom Runge-Kutta schemes. For more examples, refer to the source code in the `src/rk_tables` directory.


