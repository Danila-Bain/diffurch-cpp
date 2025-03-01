#include "../../src/util/math.hpp"
#include "../../src/util/postprocessing.hpp"
#include "../../src/util/print.hpp"
#include <iostream>

int error_count = 0;
#define ASSERT(condition)                                                      \
  if (!(condition)) {                                                          \
    cout << "Assertion failed at " << __FILE__ << ":" << __LINE__ << endl;     \
    error_count++;                                                             \
  }

using namespace std;
using namespace diffurch;

int main() {
  {
    ASSERT(sign(1.) == 1.);
    ASSERT(sign(1) == 1);
    ASSERT(sign(-1.) == -1.);
    ASSERT(sign(-1) == -1);
    ASSERT(sign(0.) == 0.);
    ASSERT(sign(0) == 0);
  }
  {
    ASSERT(step(1.) == 1.);
    ASSERT(step(1) == 1);
    ASSERT(step(-1.) == 0.);
    ASSERT(step(-1) == 0);
    ASSERT(step(0.) == 1.);
    ASSERT(step(0) == 1);
  }
  {
    ASSERT(step(1., -2., 4.) == 4.);
    ASSERT(step(-1., -2., 4.) == -2.);
    ASSERT(step(0., -2., 4.) == 4.);
  }

  if (error_count == 0) {
    cout << "All tests finished succesfully" << endl;
  } else {
    cout << error_count << " assertions failed." << endl;
  }
}
