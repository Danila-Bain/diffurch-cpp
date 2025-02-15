#pragma once

#include "primitives.hpp"
#include <type_traits>

namespace diffurch {

template <typename DetectHandler = std::nullptr_t,
          typename SaveHandler = std::nullptr_t,
          typename SetHandler = std::nullptr_t>
struct Event : EventSaveInterface<SaveHandler>, EventSetInterface<SetHandler> {
private:
  DetectHandler _detect;

public:
  Event(DetectHandler detect_handler = DetectHandler{},
        SaveHandler save_handler = SaveHandler{},
        SetHandler set_handler = SetHandler{})
      : EventSaveInterface<SaveHandler>(save_handler),
        EventSetInterface<SetHandler>(set_handler), _detect(detect_handler) {};

  void operator()(auto &state) {
    constexpr bool can_save = !std::is_same_v<SaveHandler, std::nullptr_t>;
    constexpr bool can_set = !std::is_same_v<SetHandler, std::nullptr_t>;

    if constexpr (can_save) {
      this->save(state);
    }
    if constexpr (can_set) {
      this->set(state);
      if constexpr (can_save) {
        this->save(state);
      }
    }
  }
};

/*template <typename A1, typename A2, typename A3>*/
/*Event(A1, A2, A3) -> Event<A1, A2, A3>;*/
/*template <typename A1, typename A2>*/
/*Event(A1, A2) -> Event<A1, A2, std::nullptr_t>;*/
/*template <typename A1> Event(A1) -> Event<A1, std::nullptr_t,
 * std::nullptr_t>;*/
/*Event() -> Event<std::nullptr_t, std::nullptr_t, std::nullptr_t>;*/

/*template <typename T> struct isEvent : std::false_type {};*/
/*template <typename... T> struct isEvent<Event<T...>> : std::true_type {};*/
/*template <typename T> inline constexpr bool isEvent_v = isEvent<T>::value;*/

#define EVENT_WITHOUT_DETECTION(EventName)                                     \
  template <typename SaveHandler = std::nullptr_t,                             \
            typename SetHandler = std::nullptr_t>                              \
  struct EventName : Event<std::nullptr_t, SaveHandler, SetHandler> {          \
    EventName(SaveHandler save_handler = SaveHandler{},                        \
              SetHandler set_handler = SetHandler{})                           \
        : Event<std::nullptr_t, SaveHandler, SetHandler>(                      \
              nullptr, save_handler, set_handler) {}                           \
  };                                                                           \
/*template <typename T> struct is##EventName : std::false_type {}; \*/
/*template <typename... T> \*/
/*struct is##EventName<EventName<T...>> : std::true_type {}; \*/
/*template <typename T> \*/
/*inline constexpr bool is##EventName##_v = is##EventName<T>::value;*/

EVENT_WITHOUT_DETECTION(StepEvent);   // Event called after each accepted step
EVENT_WITHOUT_DETECTION(RejectEvent); // Event after each rejected step

EVENT_WITHOUT_DETECTION(CallEvent); // Event after each rhs function call

EVENT_WITHOUT_DETECTION(StartEvent); // Event befor integration start
EVENT_WITHOUT_DETECTION(StopEvent);  // Event after integration stop

/*CallEvent(nullptr, Var(callcount)++);*/
} // namespace diffurch
