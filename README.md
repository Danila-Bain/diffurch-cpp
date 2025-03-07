# Diffurch

Diffurch is an in-development header-only c++ library that implements numerical methods for solving differential equations, with main focus on the applicability to many types of problems and usability. Having in mind that main users would be mathematicians. 

Equation type support:
[x] ordinary differential equations (such as `x' = x/t + x*x`, `x'' = sin(x)`, or `x'' + x = cos(2*t)`)
[x] delay differential equations (such as `x' = sin(x(t - 1))`)
[x] neutral delay differential equations (such as `x' = x'(t - 1)`)
[x] discontinuous differential equations (such as `x' = -y; y' = sign(x)` or `x'(t) = -sign(x(t-1))`)
[ ] differential equations with delta functions (such as `x' = -x + delta(sin(t))`)
[ ] hybrid differential equations (such as [`x' = -x + q`](https://www.researchgate.net/publication/342725034_Exact_analytic_solution_for_a_chaotic_hybrid_dynamical_system_and_its_electronic_realization), where `q` is a descrete variable that becomes `2` after `x` passes through `-1` from above and becomes `-2` after `x` passes through `1` from below) 
[x] any mix of the above

Links:
- See [documentation](docs/index.md)

# Usage

## Example: Lorenz System

### x-z projection of attractor

```c++
// lorenz.cpp

#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

struct Lorenz : diffurch::Solver<Lorenz> {

  // parameters in equation
  double sigma, rho, beta;

  Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
      : sigma(sigma_), rho(rho_), beta(beta_) {};

  // define the right-hand side of the equation
  auto get_rhs() {
    using namespace diffurch::variables_xyz_t; // defines symbols x,y,z, and t
    return sigma * (y - x) | x * (rho - z) - y | x * y - beta * z;
  }
  //
  // define an initial condition (this can also be parametrized)
  auto get_ic() { return diffurch::Constant(1.1) | 1.2 | 20.; }
};

namespace plt = matplotlibcpp;

int main(int, char *[]) {

  Lorenz eq;

  // compute the solution from t=0 to t=50, with stepsize 0.005
  auto sol = eq.solution(0, 50, diffurch::ConstantStepsize(0.005));
  auto [t, x, y, z] = sol; // unpack the tuple of vectors

  plt::plot(x, z, {{"color", "b"}, {"linewidth", "1"}});
  plt::tight_layout();
  plt::show();

  return 0;
}
```


### Lorenz map 

```c++
// lorenz_map.cpp

#include "../diffurch.hpp" // for solving
#include <matplotlibcpp.h> // for plotting

namespace plt = matplotlibcpp;
using namespace diffurch::variables_xyz_t; // defines symbols x,y,z, and t
using namespace diffurch;

int main(int, char *[]) {

    struct Lorenz : Solver<Lorenz> {
        Lorenz(double sigma_ = 10., double rho_ = 28., double beta_ = 8. / 3.)
        : sigma(sigma_), rho(rho_), beta(beta_) {};
        auto get_rhs() {
            return sigma * (y - x) | x * (rho - z) - y | x * y - beta * z;
        }
        auto get_ic() { return Constant(1.1) | 1.2 | 20.; }
    } eq;

    // compute the solution from 0 to 500, saving the values of t and z 
    // when z' = x*y - beta*z goes from positive to negative,
    // i.e. save the sequence of local maxima of z
    auto sol =
        eq.solution(0, 500, diffurch::ConstantStepsize(0.01),
                    std::make_tuple(Event(When(x * y - eq.beta * z < 0), t | z)));
    auto [tmax, zmax] = sol; // unpack the tuple of vectors

    // plot points (z[n], z[n+1])
    plt::scatter(std::vector(zmax.begin(), zmax.end() - 1),
                 std::vector(zmax.begin() + 1, zmax.end()));

    // plot the diagonal
    plt::plot(zmax, zmax);

    plt::tight_layout();
    plt::show();

    return 0;
}
```
