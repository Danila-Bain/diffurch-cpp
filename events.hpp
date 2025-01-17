#include "type_traits.hpp"
#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <vector>

struct EventBase {};

/*
 Event is an object that is described by the type of detection ("when", "step",
 "reject", "stop_integration", etc), and callbacks, that are optional,
 can coexist and can be of the types "save" and "change". If both "save" and
 "change" callbacks are specified, then "save" is called both before and after
 the change. "save" evaluates and stores current state information, "change"
 reassigns current state variables in state, it can't modify time variable or
 past states.

 What I want is to specify event detection, event save action, and event change
 action in separate classes and then combine them like
 ``auto event = When(x = 0 && y < 0) | Save(t) | Change( y = -y ) ``.
 With this approach, It is easier to combine those optional save and change
 things, and use other types of detection in place of "When". Otherwise it would
 be something like ``auto event = Event(x = 0, t, y = -y)`` if all parts are
 specified, and auto event = Event(x = 0, nullopt, nullopt) or something like
 that if save and change handlers are ommited. I don't like that,
 it is much less readable and less customizable.

But I don't like this approach either, because it doesn't make sense to combine
several detection parts, several save parts, or several change parts, because
several when's make codimension-2 event, which do not happen generally, several
saves are better handled trough several arguments, like Save(t, x, y, z) or
Save(t, {x,y,z}), and the same for change --- Change(y = -y | z = 2*z);

Then, it is super inconvinient to implement. So actually Event(x = 0 && y < 0,
Save(t, x, y, z), y = -y) syntax is more desirable, the only question is how to
sensibly delimit actual when, save, and change parts, so this is more readable.
Just `Event(When(x = 0 && y < 0), Save(t, x, y, z), Change(y = -y))`? Maybe in
this case it is sufficient to have positional arguments, maybe with defaults,
because there are only 3 arguments

Event(x == 0 && y < 0, {t, x, y, z}, y = -y && z = -z)

Event(x == 0 && y < 0, All, None)

St epEvent(All, [&stepcount](){stepcount++;})
StepRejectedEvent(Vec(t, stepsize, error))
Event(x == 0, {All, x[t-1]}, StopIntegration)
StopEvent(t)
 */

/*SaveT is either a tuple, or not, if not*/

template <typename DetectionHandler = std::nullptr_t>
struct EventDetectionInterface {
  DetectionHandler detection_handler;

  EventDetectionInterface(DetectionHandler detection_handler_)
      : detection_handler(detection_handler_) {};

  bool detect(const auto &state) { return detection_handler.detect(state); }
  double locate(const auto &state) { return detection_handler.locate(state); }
};

template <> struct EventDetectionInterface<std::nullptr_t> {
  EventDetectionInterface(std::nullptr_t _ = nullptr) {}
};

template <typename SaveHandler = std::nullptr_t> struct EventSaveInterface {
private:
  SaveHandler save_handler;

public:
  EventSaveInterface(SaveHandler save_handler_)
      : save_handler(save_handler_) {};

  std::vector<double> saved;
  void save(const auto &state) { saved.push_back(to_save(state)); }
};

template <> struct EventSaveInterface<std::nullptr_t> {
  EventSaveInterface(std::nullptr_t _ = nullptr) {}
};

template <typename... SaveHandlers>
struct EventSaveInterface<std::tuple<SaveHandlers...>> {
private:
  std::tuple<SaveHandlers...> save_handlers;
  template <size_t... index> void save_impl(const auto &state) {
    (std::get<index>(saved).push_back(std::get<index>(save_handlers)(state)),
     ...);
  }

public:
  EventSaveInterface(std::tuple<SaveHandlers...> save_handlers_)
      : save_handlers(save_handlers_) {};

  std::tuple<std::conditional_t<true, std::vector<double>, SaveHandlers>...>
      saved;

  void save(const auto &state) {
    save_impl<std::index_sequence_for<SaveHandlers...>>(state);
  }
};

template <typename SetHandler = std::nullptr_t> struct EventSetInterface {

  SetHandler set_handler;

  EventSetInterface(SetHandler set_handler_) : set_handler(set_handler_) {};

  void set(auto &state)
    requires(!std::is_same_v<SetHandler, std::nullptr_t>)
  {
    if constexpr (requires { set_handler(state); })
      set_handler(state);
    else
      set_handler();
  }
};

template <typename DetectT = std::nullptr_t,
          typename SaveHandler = std::nullptr_t,
          typename SetHandler = std::nullptr_t>
struct Event : EventSaveInterface<SaveHandler>, EventSetInterface<SetHandler> {
private:
  DetectT _detect;

public:
  Event(DetectT detect_ = nullptr, SaveHandler save_handler = nullptr,
        SetHandler set_handler = nullptr)
      : _detect(detect_), EventSaveInterface<SaveHandler>(save_handler),
        EventSetInterface<SetHandler>(set_handler) {};

  void operator()(auto &state) {
    constexpr bool can_save = requires { save(state); };
    constexpr bool can_set = requires { set(state); };
    if constexpr (can_save) {
      save(state);
    }
    if constexpr (can_set) {
      set(state);
      if constexpr (can_save) {
        save(state);
      }
    }
  }
};

/*template <typename T> struct isEvent : std::false_type {};*/
/*template <typename... T> struct isEvent<Event<T...>> : std::true_type {};*/
/*template <typename T> inline constexpr bool isEvent_v = isEvent<T>::value;*/

#define EVENT_WITHOUT_DETECTION(EventName)                                     \
  template <typename SaveHandler = std::nullptr_t,                             \
            typename SetHandler = std::nullptr_t>                              \
  struct EventName : Event<std::nullptr_t, SaveHandler, SetHandler> {          \
    EventName(SaveHandler save_handler = nullptr,                              \
              SetHandler set_handler = nullptr)                                \
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

// Class that contains events that will run simultaniously
template <typename... EventTypes>
/*requires(is_kind_of_v<EventTypes, EventType> && ...)*/
struct SimultaniousEvents {
  std::tuple<EventTypes...> event_tuple;

  SimultaniousEvents(const std::tuple<EventTypes...> &event_tuple_)
      : event_tuple(event_tuple_) {};
  /*SimultaniousEvents(const EventTypes &...events_)*/
  /*    : event_tuple(std::make_tuple(events_...)) {};*/

  // run event(state) for all events in event_tuple
  void operator()(auto &state) {
    std::apply([&state](auto &&...events) { (events(state), ...); },
               event_tuple);
  }
};

template <template <typename...> typename EventType, typename... Ts>
struct filter_events_impl {
  template <typename T> using Condition = is_kind_of<T, EventType>;
  static constexpr auto impl(const std::tuple<Ts...> &events) {
    return filter_tuple<Condition>(events);
  }
};

template <template <typename...> typename EventType, typename... Ts>
constexpr auto filter_events(const std::tuple<Ts...> &events) {
  return filter_events_impl<EventType, Ts...>::impl(events);
}

template <template <typename...> typename EventType, typename... Ts>
constexpr auto filter_events(const Ts &...events) {
  return filter_events<EventType, Ts...>(std::make_tuple(events...));
}

template <template <typename...> typename EventType, typename... EventTypes>
using filter_events_t =
    decltype(filter_events<EventType>(std::declval<EventTypes>()...));

template <template <typename...> typename EventType, typename... EventTypes>
auto filter_simultaneous_events(const std::tuple<EventTypes...> &events) {
  return SimultaniousEvents(filter_events<EventType, EventTypes...>(events));
  /*return std::apply(*/
  /*    [&](auto... args) { return SimultaniousEvents<EventType>(args...); },*/
  /*    filter_events<EventType, EventTypes...>(events));*/
}

template <template <typename...> typename EventType, typename... EventTypes>
auto filter_simultaneous_events(const EventTypes &...events) {
  return SimultaniousEvents(filter_events<EventType, EventTypes...>(events...));
}

template <template <typename...> typename EventType, typename... EventTypes>
using filter_simultaneous_events_t =
    decltype(filter_simultaneous_events<EventType, EventTypes...>(
        std::make_tuple(std::declval<EventTypes>()...)));

template <typename... EventTypes> struct Events {
private:
  size_t located_event_index = -1;

public:
  filter_events_t<Event, EventTypes...> detection_events;

  filter_simultaneous_events_t<StepEvent, EventTypes...> step_events;
  filter_simultaneous_events_t<RejectEvent, EventTypes...> reject_events;
  filter_simultaneous_events_t<CallEvent, EventTypes...> call_events;
  filter_simultaneous_events_t<StartEvent, EventTypes...> start_events;
  filter_simultaneous_events_t<StopEvent, EventTypes...> stop_events;

  Events(EventTypes... events) : Events(std::make_tuple(events...)) {}
  /*Events() : Events(std::tuple<>{}) {}*/

  Events(const std::tuple<EventTypes...> &events)
      : detection_events(filter_events<Event>(events)),
        step_events(filter_simultaneous_events<StepEvent>(events)),
        reject_events(filter_simultaneous_events<RejectEvent>(events)),
        call_events(filter_simultaneous_events<CallEvent>(events)),
        start_events(filter_simultaneous_events<StartEvent>(events)),
        stop_events(filter_simultaneous_events<StopEvent>(events)) {}

  template <typename... EventTypes1, typename... EventTypes2>
  Events(Events<EventTypes1...> events1, Events<EventTypes2...> events2)
      : detection_events(
            std::tuple_cat(events1.detection_events, events2.detection_events)),
        step_events(SimultaniousEvents(std::tuple_cat(
            events1.step_events.event_tuple, events2.step_events.event_tuple))),
        reject_events(SimultaniousEvents(
            std::tuple_cat(events1.reject_events.event_tuple,
                           events2.reject_events.event_tuple))),
        call_events(SimultaniousEvents(std::tuple_cat(
            events1.call_events.event_tuple, events2.call_events.event_tuple))),
        start_events(SimultaniousEvents(
            std::tuple_cat(events1.start_events.event_tuple,
                           events2.start_events.event_tuple))),
        stop_events(SimultaniousEvents(
            std::tuple_cat(events1.stop_events.event_tuple,
                           events2.stop_events.event_tuple))) {}

  template <typename... EventTypes1, typename... EventTypes2, typename... Rest>
  Events(Events<EventTypes1...> events1, Events<EventTypes2...> events2,
         Rest... rest)
      : Events(Events(events1, events2), rest...) {}

  double locate(const auto &state) {
    static_assert(false);
    double event_t = std::numeric_limits<double>::max();

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (
          [&state, &event_t, this](auto &&event, size_t index) {
            double t = event.locate(state);
            if (t < event_t) {
              event_t = t;
              located_event_index = index;
            }
          }(std::get<Is>(detection_events), Is),
          ...);
    }(std::make_index_sequence<
        std::tuple_size_v<decltype(detection_events)>>{});

    return event_t;
  }

  void located_event(auto &state) {
    if (located_event_index == -1)
      return;

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (
          [&state, this](auto &&event, size_t index) {
            if (index == located_event_index) {
              event(state);
              located_event_index = -1;
            }
          }(std::get<Is>(detection_events), Is),
          ...);
    }(std::make_index_sequence<
        std::tuple_size_v<decltype(detection_events)>>{});
  }
};

/*
 in solver:
    double t = events.locate(state);
    redo_step_to(t);
    events.located_call_back(state);
 */

/*                                                                               \
 Events events(StepEvent(All), Event(x==0, x[t-1], x=x+1), Event(y==0, null, \
 y=y-1), Event(z==0)) \
                                                                               \ \
 events.step_events; \
 events.when_events; \
 events.rejected_step_events; \
 events.stop_events; \
                                                                               \ \
 are tuples with corresponding events sorted out \
                                                                               \ \
 all types of events except when_events are all run in any order when needed, \
 it is easy to implement: some foreach function for tuples is needed \
                                                                               \ \
 when_events need additional detection/location step, in which \
 the minimal located event time is chosen and other events are \
 ignored. Not sure how to implement that with tuples. \
    * find index of minimal event time and \
      than swich-case to call callback of minimal event? \
    * save pointers to minimal event and its callback method \
      as void* and void (*)(void*, state), then call it? \
      This is dirty and potentially will break because it relies on undefined \
      behavior. \
                                                                               \ \
 saving is done by evaluating state at current moment, and storing the result \
 in a vector (easy), the tricky part is to recover all saved information. \
                                                                               \ \
 the output shall be specified in a predictable manner: it should be defined, \
 which event outputs to where. The proposition is that solver will output the \
 events.get_saved() result, which is a struct: \
 struct EventOutput { \
     tuple<...> step_events_output; \
     tuple<...> when_events_output; \
     ... \
 } \
 where each member is a tuple in which each element corresponds to output of \
 some event in the same order in which it appeared in declaration. \
                                                                               \ \
 auto [steps, whens, stops, rejected] = solver.solve(...); \
 auto [t, x, y, z] = steps.first \
                                                                               \ \
 */

/*template <typename... EventTypes> struct Events {*/
/*  tuple<EventTypes...> events;*/
/**/
/*  tuple<EventTypes...> step_events;*/
/*  tuple<EventTypes...> when_events;*/
/*  tuple<EventTypes...> stop_events;*/
/*};*/

/*template <typename Event1, typename Event2> struct EventUnion {*/
/*  Event1 e1;*/
/*};*/

/* When(x == 0 && y < 0)
 * WhenStep
 * RejectEvent
 * WhenStopIntegration */

/*template <typename T>*/
/*struct When {*/
/**/
/*};*/

/*struct WhenStep {};*/
/*struct WhenZero {};*/

// auto example = When(x == 0 && y < 0) | Save(t);

/*template <typename EventLocator> struct When : EventBase {*/
/*  EventLocator event_locator;*/
/*  When(EventLocator event_locator_) : event_locator(event_locator_) {};*/
/**/
/*  auto detect(const auto &state) { event_locator.detect(state); }*/
/**/
/*  auto locate(const auto &state) { event_locator.locate(state); }*/
/*};*/
/**/
/*template <typename EventSaver> struct Save : EventBase {*/
/*  EventSaver event_saver;*/
/*  Save(EventSaver event_saver_) : event_saver(event_saver_) {};*/
/**/
/*  std::vector<decltype(event_saver.save(StateClassPlaceHolder{}))>
 * container;*/
/**/
/*  void save(const auto &state) { container.push_back(event_saver.save(state));
 * }*/
/*};*/

/*


  Events events(WhenEqualZero(x - y) | WhileGreaterZero(x + y) | Save(t) |
                    Set(counter++),
                WhenStep() | Save(t, x, y, z),
                WhenGreater(abs(x), 1000) | StopIntegration(),
                WhenStopIntegration() | Save(make_tuple(x, y, z)),
                WhenStepRejected() | Save(t) |
                    Set([&]() { rejected_counter++ }),
                WhenZero(z[t - 1]));



 */

/*struct WhenStep {};*/
/**/
/*template <typename T> struct Save {*/
/*  T what_to_save;*/
/**/
/*  Save(T f) : what_to_save(f) {};*/
/*};*/
/**/
/*struct Set {};*/

/*auto event = When(x == 0 && y < 0) | Save(x, y) | Save(t) | Change((x = 1) &&
 * (y = x + z));*/

/*double event_t = event.locate(state);*/
/*event.callback(state);*/
/*event.get_saved();*/

/*struct WhenStep {};*/
/*struct WhenStepRejected {};*/

/*When(x == 0) | Save(t) | Set(y = -y)*/

/*When(... = 0) | Set(_x = _x + (lhs1 - lhs0)*Db(x) * _x[t-1] / (abs(Db(x) *
 * lhs[t-1]) )  )*/
