#pragma once

#include "../symbolic/vector.hpp"
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
namespace diffurch {
template <typename SaveHandler = std::nullptr_t> struct EventSaveInterface {
private:
  SaveHandler save_handler;

public:
  EventSaveInterface(const SaveHandler &save_handler_)
      : save_handler(save_handler_) {};
  std::tuple<std::vector<double>> saved;
  void save(const auto &state) {
    std::get<0>(saved).push_back(save_handler(state));
  }
};

template <> struct EventSaveInterface<std::nullptr_t> {
  EventSaveInterface(std::nullptr_t = nullptr) {};
  std::tuple<> saved;
  inline void save(const auto &state) {}
};

template <typename... SaveHandlers>
struct EventSaveInterface<std::tuple<SaveHandlers...>> {
private:
  std::tuple<SaveHandlers...> save_handlers;

  template <std::size_t... index>
  void save_impl(const auto &state, std::index_sequence<index...>) {
    (std::get<index>(saved).push_back(std::get<index>(save_handlers)(state)),
     ...);
  }

public:
  EventSaveInterface(const std::tuple<SaveHandlers...> &save_handlers_)
      : save_handlers(save_handlers_) {};

  std::tuple<std::conditional_t<true, std::vector<double>, SaveHandlers>...>
      saved;

  void save(const auto &state) {
    save_impl(state, std::index_sequence_for<SaveHandlers...>{});
  }
};

template <typename... SaveHandlers>
struct EventSaveInterface<Vector<SaveHandlers...>>
    : EventSaveInterface<std::tuple<SaveHandlers...>> {

  EventSaveInterface(const Vector<SaveHandlers...> &vector_)
      : EventSaveInterface<std::tuple<SaveHandlers...>>(vector_.coordinates) {};
};

template <typename SetHandler = std::nullptr_t> struct EventSetInterface {
  SetHandler set;
  EventSetInterface(const SetHandler &set_) : set(set_) {};
};

template <typename DetectionHandler = std::nullptr_t>
struct EventDetectionInterface {
  DetectionHandler detection_handler;

  EventDetectionInterface(DetectionHandler detection_handler_)
      : detection_handler(detection_handler_) {};

  bool detect(const auto &state) { return detection_handler.detect(state); }
  double locate(const auto &state) { return detection_handler.locate(state); }
};

template <> struct EventDetectionInterface<std::nullptr_t> {
  EventDetectionInterface(std::nullptr_t = nullptr) {}
};
} // namespace diffurch
