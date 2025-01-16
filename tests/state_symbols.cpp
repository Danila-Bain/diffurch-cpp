
#include "../state_symbols.hpp"
#include "../print.hpp"
#include "../vec.hpp"
#include <cassert>
#include <iostream>

using namespace std;

using namespace State;

template <size_t n> struct MyState {
  double prev_t;
  double curr_t;
  Vec<n> prev_x;
  Vec<n> curr_x;

  template <size_t derivative, size_t index = -1> auto eval(double t) const {
    return t + double(index) * 1000;
  }
};

int main(int argc, char *argv[]) {

  /*MyState state{Vec<3>{1, 2, 3}, 11, Vec<3>{1.1, 2.2, 3.3}, 21};*/

  { // basic test
    State::Variable<0> x;
    State::Variable<1> y;
    State::Variable<2> z;

    static_assert(IsStateExpression<decltype(x)>);
    static_assert(IsStateExpression<decltype(z)>);
    static_assert(IsStateExpression<decltype(x + y)>);
    static_assert(IsStateExpression<decltype(x + 1)>);
    static_assert(IsStateExpression<decltype(x[x + 1])>);
  }

  {
    auto t = State::TimeVariable();
    auto [x, y, z] = State::Variables<3>();
    auto state = MyState<3>{.prev_t = 0,
                            .curr_t = 1,
                            .prev_x = Vec<3>{1, 2, 3},
                            .curr_x = Vec<3>{10, 20, 30}};

    assert(x(state) == 10);
    assert(y(state) == 20);
    assert(z(state) == 30);
    assert(z.prev(state) == 3);

    assert(cos(x - 10)(state) == 1);
    assert((x + y).prev(state) == 3);
    assert((y * z)(state) == 600);
    assert(exp(z - x - y)(state) == 1);

    assert((Vector(x, y, z)(state)) == (Vec<3>{10, 20, 30}));
    assert((Vector(y - x, x - y, z - x - y)(state)) == (Vec<3>{10, -10, 0}));

    assert(x[t](state) == t(state));
    assert(x[t + 30](state) == (t + 30)(state));
    assert(y[t + 30](state) == 1000 + (t + 30)(state));
    assert(z[t - x](state) == 2000 + t(state) - x(state));
  }

  cout << "Execution finished normally" << endl;

  return 0;
}
