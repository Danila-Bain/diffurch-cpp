#pragma once

#include "../util/type_traits.hpp"
#include "event.hpp"
#include <limits>
#include <tuple>

namespace diffurch {

template <template <typename...> typename EventType, typename... EventTypes>
struct SimultaniousEvents {
  std::tuple<EventTypes...> event_tuple;

  SimultaniousEvents(const SimultaniousEvents<EventType, EventTypes...> &events)
      : event_tuple(events.event_tuple) {};
  SimultaniousEvents(const std::tuple<EventTypes...> &event_tuple_)
      : event_tuple(event_tuple_) {};
  SimultaniousEvents(const EventTypes &...events_)
      : event_tuple(std::make_tuple(events_...)) {};

  template <typename... EventTypes1, typename... EventTypes2>
  SimultaniousEvents(const SimultaniousEvents<EventType, EventTypes1...> &se1,
                     const SimultaniousEvents<EventType, EventTypes2...> &se2)
      : event_tuple(std::tuple_cat(se1.event_tuple, se2.event_tuple)){};

  // run event(state) for all events in event_tuple
  void operator()(auto &state) {
    std::apply([&state](auto &&...events) { (events(state), ...); },
               event_tuple);
  }
};

template <template <typename...> typename EventType, typename... EventTypes>
SimultaniousEvents(const SimultaniousEvents<EventType, EventTypes...> &events)
    -> SimultaniousEvents<EventType, EventTypes...>;

template <template <typename...> typename EventTemplate, typename... Args,
          typename... EventTypes>
SimultaniousEvents(const std::tuple<EventTemplate<Args...>, EventTypes...> &)
    -> SimultaniousEvents<EventTemplate, EventTemplate<Args...>, EventTypes...>;

template <template <typename...> typename EventTemplate,
          typename... EventTypes1, typename... EventTypes2>
SimultaniousEvents(const SimultaniousEvents<EventTemplate, EventTypes1...> &se1,
                   const SimultaniousEvents<EventTemplate, EventTypes2...> &se2)
    -> SimultaniousEvents<EventTemplate, EventTypes1..., EventTypes2...>;

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
  if constexpr (std::is_same_v<std::tuple<>,
                               filter_events_t<EventType, EventTypes...>>) {
    return SimultaniousEvents<EventType>(std::tuple<>());
  } else {
    return SimultaniousEvents(filter_events<EventType, EventTypes...>(events));
  }
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
  size_t located_event_index = -1;

  filter_events_t<Event, EventTypes...> detection_events;

  filter_simultaneous_events_t<StepEvent, EventTypes...> step_events;
  filter_simultaneous_events_t<RejectEvent, EventTypes...> reject_events;
  filter_simultaneous_events_t<CallEvent, EventTypes...> call_events;
  filter_simultaneous_events_t<StartEvent, EventTypes...> start_events;
  filter_simultaneous_events_t<StopEvent, EventTypes...> stop_events;

  Events(EventTypes... events) : Events(std::make_tuple(events...)) {}

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
        step_events(events1.step_events, events2.step_events),
        reject_events(events1.reject_events, events2.reject_events),
        call_events(events1.call_events, events2.call_events),
        start_events(events1.start_events, events2.start_events),
        stop_events(events1.stop_events, events2.stop_events) {}

  template <typename... EventTypes1, typename... EventTypes2, typename... Rest>
  Events(Events<EventTypes1...> events1, Events<EventTypes2...> events2,
         Rest... rest)
      : Events(Events(events1, events2), rest...) {}

  double locate(const auto &state) {
    double t_event = std::numeric_limits<double>::max();

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (
          [&state, &t_event, this](auto &&event, size_t index) {
            double t = event.locate(state);
            if (t < t_event) {
              t_event = t;
              located_event_index = index;
            }
          }(std::get<Is>(detection_events), Is),
          ...);
    }(std::make_index_sequence<
        std::tuple_size_v<decltype(detection_events)>>{});

    return t_event;
  }

  void located_event(auto &state) {
    if (located_event_index == size_t(-1))
      return;

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      (
          [&state, this](auto &&event, size_t index) {
            if (index == located_event_index) {
              if (event.detect(state)) // if event is still detected
                event(state);
              located_event_index = -1;
              return;
            }
          }(std::get<Is>(detection_events), Is),
          ...);
    }(std::make_index_sequence<
        std::tuple_size_v<decltype(detection_events)>>{});
  }

  auto get_saved() const {
    auto get_saved_from_tuple = [](const auto &tuple_) {
      return std::apply(
          [](const auto &...event_tuple) {
            return std::tuple_cat(event_tuple.saved...);
          },
          tuple_);
    };

    return tuple_cat(get_saved_from_tuple(detection_events),
                     get_saved_from_tuple(step_events.event_tuple),
                     get_saved_from_tuple(reject_events.event_tuple),
                     get_saved_from_tuple(call_events.event_tuple),
                     get_saved_from_tuple(start_events.event_tuple),
                     get_saved_from_tuple(stop_events.event_tuple));
  }
};

template <typename... EventTypes1, typename... EventTypes2>
Events(Events<EventTypes1...> events1, Events<EventTypes2...> events2)
    -> Events<EventTypes1..., EventTypes2...>;
} // namespace diffurch
