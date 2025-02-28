
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
    vector<double> v{0, 1};
    auto [c, h] = hist(v, 2);
    ASSERT((h == vector<double>{1, 1}));
  }
  {
    vector<double> v{1, 0};
    auto [c, h] = hist(v, 2);
    ASSERT((h == vector<double>{1, 1}));
  }
  {
    vector<double> v{0, 1};
    auto [c, h] = hist(v, 3);
    ASSERT((h == vector<double>{1, 0, 1}));
  }
  {
    vector<double> v{0, 0.1, 0.2, 0.5, 1 - 0.1, 1};
    auto [c, h] = hist(v, 3);
    ASSERT((h == vector<double>{3, 1, 2}));
  }
  {
    vector<double> v{0, 0.5, 1};
    auto [c, h] = hist(v, 2);
    ASSERT((h == vector<double>{2, 1}));
  }
  {
    vector<double> v{0, 0.49, 1};
    auto [c, h] = hist(v, 2);
    ASSERT((h == vector<double>{2, 1}));
  }
  {
    vector<double> v{0, 0.25, 1};
    auto [c, h] = hist(v, 4);
    ASSERT((h == vector<double>{2, 0, 0, 1}));
  }
  {
    vector<double> v{0, 0.251, 1};
    auto [c, h] = hist(v, 4);
    ASSERT((h == vector<double>{1, 1, 0, 1}));
  }

  if (error_count == 0) {
    cout << "All tests finished succesfully" << endl;
  } else {
    cout << error_count << " assertions failed." << endl;
  }
}
