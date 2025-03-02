#pragma once

#include "primitives.hpp"
#include <type_traits>
#include <utility>

namespace diffurch {

template <typename DetectHandler = std::nullptr_t,
          typename SaveHandler = std::nullptr_t,
          typename SetHandler = std::nullptr_t>
struct Event : EventDetectionInterface<DetectHandler>,
               EventSaveInterface<SaveHandler>,
               EventSetInterface<SetHandler> {
private:
public:
  Event(const DetectHandler &detect_handler = DetectHandler{},
        const SaveHandler &save_handler = SaveHandler{},
        const SetHandler &set_handler = SetHandler{})
      : EventSaveInterface<SaveHandler>(save_handler),
        EventSetInterface<SetHandler>(set_handler),
        EventDetectionInterface<DetectHandler>(detect_handler) {};

  // Event action, constiting of calling save and set handlers.
  void operator()(auto &state) {
    static constexpr bool is_saving = requires { this->save(state); };
    static constexpr bool is_setting_no_args = requires { this->set(); };
    // rvalue binds only if set accepts state by copy or const reference
    static constexpr bool is_setting_const_state =
        requires { this->set(std::move(state)); };
    static constexpr bool is_setting_non_const_state =
        (requires { this->set(state); }) && !is_setting_const_state;

    if constexpr (is_saving) {
      this->save(state);
    }

    if constexpr (is_setting_no_args) {
      this->set();
    } else if constexpr (is_setting_const_state) {
      this->set(state);
    } else if constexpr (is_setting_non_const_state) {
      state.make_zero_step();
      this->set(state);
      if constexpr (is_saving) {
        this->save(state);
      }
    }
  }
};

#define EVENT_WITHOUT_DETECTION(EventName)                                     \
  template <typename SaveHandler = std::nullptr_t,                             \
            typename SetHandler = std::nullptr_t>                              \
  struct EventName : Event<std::nullptr_t, SaveHandler, SetHandler> {          \
    EventName(const SaveHandler &save_handler = SaveHandler{},                 \
              const SetHandler &set_handler = SetHandler{})                    \
        : Event<std::nullptr_t, SaveHandler, SetHandler>(                      \
              nullptr, save_handler, set_handler) {}                           \
  };

EVENT_WITHOUT_DETECTION(StepEvent);   // Event called after each accepted step
EVENT_WITHOUT_DETECTION(RejectEvent); // Event after each rejected step

EVENT_WITHOUT_DETECTION(CallEvent); // Event after each rhs function call

EVENT_WITHOUT_DETECTION(StartEvent); // Event befor integration start
EVENT_WITHOUT_DETECTION(StopEvent);  // Event after integration stop

} // namespace diffurch
