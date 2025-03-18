#include <iostream>

#include "../../src/events.hpp"
#include "../../src/symbolic.hpp"
#include "../../src/util/print.hpp"
#include <cstddef>
#include <tuple>

using namespace std;
using namespace diffurch;
using namespace diffurch::variables_xyz_t;

struct StateLike {
  double t_curr = 42.;
  double t_prev = 42.;
  array<double, 3> x_curr{0.01, 0.02, 0.03};
  array<double, 3> x_prev{-0.01, -0.02, -0.03};
  void make_zero_step() {
    t_prev = t_curr;
    x_prev = x_curr;
  };
} state;

int error_count = 0;

#define ASSERT(condition)                                                      \
  if (!(condition)) {                                                          \
    cout << "Assertion failed at " << __FILE__ << ":" << __LINE__ << endl;     \
    error_count++;                                                             \
  }

int main() {
  {
    auto c = Constant(42.);
    ASSERT(c(state) == 42.);
    ASSERT(c.prev(state) == 42.);
    ASSERT(c(0.) == 42.);
    ASSERT(c(state, 0.) == 42.);
    ASSERT(c(nullptr) == 42.)
  }

  {
    double x = 42.;
    auto c = Constant(x);
    ASSERT(c(0) == 42.);
    x = 69.;
    ASSERT(c(0) == 42.);
  }

  { // conversion back to double
    auto c = Constant(4.2);
    ASSERT(c(0) == 4.2);
    double x = c;
    ASSERT(x == 4.2);
  }

  {
    auto f = t * t;
    ASSERT(f(4.) == 4. * 4.);
  }

  {
    auto f = t * t;
    ASSERT(D(f)(4.) == 2. * 4.);
  }

  {
    auto f = 3. * sin(2. * t);
    ASSERT(D(f)(4.) == 3. * cos(2. * 4.) * 2.);
    ASSERT(D(f)(4.) == 3. * 2. * cos(2. * 4.));
  }

  if (error_count == 0) {
    cout << "All tests finished succesfully" << endl;
  } else {
    cout << error_count << " assertions failed." << endl;
  }
  return 0;
}
