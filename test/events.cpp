#include "../src/events.hpp"
#include "../src/symbolic.hpp"
#include "../src/util/print.hpp"
#include <cstddef>
#include <tuple>

using namespace std;
using namespace diffurch;
using namespace diffurch::variables_xyz_t;

struct StateLike {
  double t_curr = 42.;
  array<double, 3> x_curr{0.01, 0.02, 0.03};
} state;

#define ASSERT(condition)                                                      \
  if (!(condition)) {                                                          \
    cout << "Assertion failed at " << __FILE__ << ":" << __LINE__ << endl;     \
    return 1;                                                                  \
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
  return 0;
}
