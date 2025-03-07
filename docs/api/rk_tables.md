# Runge Kutta tables

## Introduction

Runge Kutta methods present a wide family of numerical methods for solutions of differential equations. For a definition, refer to widipedia. Here we offer some basic ideas that are not obvious upon a first glance at those methods.

The basic idea is that we search for the good method in the particular class of formulas, in which there are a lot of parameters. Then, the parameters are solved for, such that the taylor series of the obtained solution approximation (as the function of the stepsize), matches the taylor series of the actual solution for as many terms as possible. If those taylor expansions match up to $n$-th power of the stepsize $h$, then the method has order of $n$, at each step the local error introduced has order of $O(h^{n+1})$, and global error of the solution has the order of $O(h^n)$. This is the basic difference between different Runge Kutta schemes. 

Each Runge Kutta method constists of the several stages, say $s$. For the method of order $n$, the number of stages required is roughly proportional to $s$ for small $n$ and grows slightly faster than linear as $n$ gets large. Meanwhile, for $s$ stages, there are exactly $s$ right hand side function calls per step, and about $s^2$ additions and multiplications per step. Hence, with the increase of the number of stages, required work per step grows quadratically, but the error per step decays exponentially (and then acceptable step size grows exponentially). Because of that, higher order method are preferable, both in terms of performance (the decrease in the number of required steps overweights the increased cost per step), and in terms of precision (when there are less steps, less rounding errors accumulate; for example, it is impossible to achieve machine precision of approximated solutions with Euler method).

Arguably, the Runge Kutta methods cover all numerical methods, that are:
1. one-step, i.e. using only the previous approximation to the solution, it produces the approximation at the next instance of time (there are methods, that use previous n approximations to produce the next one, those are multi-step methods);
2. affine covariant (term is made up by me), i.e., if we make affine (linear + parallel shift) change of variables (both dependent and independent) in the differential equation, then numerical approximation will differ by exactly the same change of variables (hence the word 'covariant', i.e. 'changes accordingly').

The property 1 is desirable (comparing to multi-step methods), when we want to deal with events, discontinuities, or delay equations, which is one of the applications of this project.

The property 2 is just a sanity check. It would be weird, if the method would behave differently if we just rescale or rotate the system or rescale the time.

Each Runge Kutta method is defined by its Butcher tableau, with two possible (optional) extensions:
1. Embedded schemes have two sets of 'b' coefficients: one 'b' row is used for actual approximation to the solution, and second 'b' row provides a slightly worse approximation to the solution. By the difference between those two approximations, the local error is approximated. This error approximation is then used to choose an optimal step-size, such that when solution changes rapidly, we use small stepsize, to capture those details, and when solution changes slowly, we proceed with bigger steps without significant loss in precision.
2. Dense output chemese provide the set of polynomials, that are used in place of `b` coefficients to interpolate the solution between the endpoints of each step. Such interpolation is usefull for event detection, plotting of the solution, and crucial for solving delay differential equation, for which evaluation of the solution between the nodal points is required to obtain the solutions with good precision.

Methods with dense output are also called continuous Runge Kutta methods, opposite to descrete Runge Kutta methods.

## Runge Kutta Table classes

This project uses Runge Kutta tables in the form of the classes with following structure:

```c++

struct my_rk_table {

    // the number of stages in the method 
    constexpr static size_t s; 

    // the order of the method
    constexpr static size_t order; 

    // (optional) the order of the embedded scheme
    constexpr static size_t order_embedded; 

    // (optional) the order of the dense output (interpolation) formula
    constexpr static size_t order_interpolation;

    // table a
    constexpr static std::array<std::array<double, s-1>, s> a;

    // column c
    constexpr static std::array<double, s> c;

    // b coefficients
    constexpr static std::array<double, s> b;

    // (optional) embedded b coefficients
    constexpr static std::array<double, s> bb;

    // (optional) polynomials in dense output formula
    // here "degree" stands for a hard-coded number
    constexpr static std::array<Polynomial<degree>, s> bs;
};
```

Here, all fields that are not commented as optional, are required, and the static keyword is important everywhere. Also, here we omitted the definitions of the coefficients, which would even raise a compiler errror due to uninitialized static members. The classic Runge Kutta Method of the 4th order with 3rd order interpolant is defined as the following:
```c++
struct rk4 {

  constexpr static size_t s = 4;
  constexpr static size_t order = 4;
  constexpr static size_t order_interpolation = 3;

  constexpr static std::array<std::array<double, 3>, 4> a{
      {{}, {0.5}, {0.0, 0.5}, {0.0, 0.0, 1.0}}};

  constexpr static std::array<double, 4> c{{0.0, 0.5, 0.5, 1.0}};

  constexpr static std::array<double, 4> b{{1./6.,1./3.,1./3.,1./6.}};

  constexpr static std::array<Polynomial<3>, 4> bs{
      {{0., 1., -1.5, 2./3.}, // bs[0](theta) is `theta - 1.5 theta^2 + 2/3 theta^3`
       {0., 0., 1., -2./3.},
       {0., 0., 1., -2./3.},
       {0., 0., -0.5, 2./3.}}};
};
```

By means of defining such a class, user can use a custom runge kutta schemes. For more examples, refer to the source code in the folder `src/rk_tables`.
