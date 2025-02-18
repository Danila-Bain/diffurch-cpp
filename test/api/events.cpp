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
  { // Default constructors
    Event();
    StepEvent();
    StopEvent();
    Events();

    StepEvent();
    StepEvent(nullptr);
    StepEvent(nullptr, nullptr);
  }

  {
    auto esi =
        EventSaveInterface([](const auto &state) { return state.t_curr; });
    esi.save(state);
    ASSERT(esi.saved == make_tuple(vector<double>{42}));
  }

  { // Saving testing
    {
      auto s = StepEvent(t);
      s(state);
      s(state);
      s(state);
      ASSERT(s.saved == make_tuple(vector<double>{42, 42, 42}));
    }

    {
      auto s = Event(nullptr, x);
      s(state);
      s(state);
      ASSERT(s.saved == make_tuple(vector<double>{0.01, 0.01}));
    }

    {
      auto s = Event(nullptr, x & y);
      s(state);
      ASSERT(s.saved == make_tuple(vector<double>{0.01}, vector<double>{0.02}));
    }

    {
      auto ee = Events(StartEvent(x), StopEvent(y), StopEvent(z));
      ee.start_events(state);
      /*cout << ee.get_saved() << endl;*/
      ASSERT(ee.get_saved() == make_tuple(vector<double>{0.01},
                                          vector<double>{}, vector<double>{}));

      ee.stop_events(state);
      ASSERT(ee.get_saved() == make_tuple(vector<double>{0.01},
                                          vector<double>{0.02},
                                          vector<double>{0.03}));
    }

    {
      auto e = StepEvent(x & y);
      e(state);
      ASSERT(e.saved == make_tuple(vector<double>{0.01}, vector<double>{0.02}));
    }

    {
      auto e = StepEvent(t | x | y);
      e(state);
      ASSERT(e.saved == make_tuple(vector<double>{42}, vector<double>{0.01},
                                   vector<double>{0.02}));
    }
  }

  { // Event setting
    {
      auto s = state;
      int step_counter{};
      auto e = StepEvent(nullptr, [&]() { step_counter++; });

      e(s);
      e(s);
      e(s);

      ASSERT(step_counter == 3);

      // handler with no arguments doesn't affect state
      ASSERT(s.x_prev == state.x_prev && s.x_curr == state.x_curr);
    }

    {
      auto s = state;
      double t_finish;
      auto e = StopEvent(nullptr,
                         [&](const auto &state) { t_finish = state.t_curr; });

      e(s);

      ASSERT(t_finish == 42.);
      // handler with const auto& argument doesn't affect state
      ASSERT(s.x_curr == state.x_curr && s.x_prev == state.x_prev);
    }

    {
      auto s = state;
      auto e =
          Event(nullptr, nullptr, [](auto &state) { state.x_curr[0] += 1; });

      e(s);

      // handler with auto& argument changes state, adding zero-step with those
      // changes
      ASSERT(s.x_curr[0] == state.x_curr[0] + 1);
      ASSERT(s.x_curr[1] == state.x_curr[1]);
      ASSERT(s.x_prev == state.x_curr);
    }

    {
      auto s = state;

      auto e1 = Event(nullptr, nullptr, x << x + 1 && y << y + 1);
      e1(s);
      ASSERT(s.x_curr[0] == 0.01 + 1. && s.x_curr[1] == 0.02 + 1.);

      auto e2 = Event(nullptr, nullptr, x << x + 1 | y << 2);
      e2(s);
      ASSERT(s.x_curr[0] == 0.01 + 1. + 1. && s.x_curr[1] == 2.);
    }

    {
      auto s = state;

      auto e1 = Event(nullptr, x | y, x << x + z);
      e1(s);

      /*cout << s.x_curr << s.x_prev << endl;*/
      /*cout << state.x_curr << state.x_prev << endl;*/
      /*cout << e1.saved << endl;*/

      ASSERT(e1.saved == make_tuple(vector<double>{0.01, 0.01 + 0.03},
                                    vector<double>{0.02, 0.02}));

      ASSERT(s.x_prev == state.x_curr);
      ASSERT((s.x_curr == array<double, 3>({0.01 + 0.03, 0.02, 0.03})));
    }
  }

  if (error_count == 0) {
    cout << "All tests finished succesfully" << endl;
  } else {
    cout << error_count << " assertions failed." << endl;
  }
  return 0;
}
