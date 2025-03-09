#pragma once

#include "symbol_types.hpp"
#include "variables.hpp"
#include <cstddef> // for size_t
#include <tuple>

namespace diffurch {

template <IsSymbol... Coordinates> struct Vector : Symbol {
  std::tuple<Coordinates...> coordinates;

  Vector(Coordinates... coordinates_)
      : coordinates(std::make_tuple(coordinates_...)) {};
  Vector(std::tuple<Coordinates...> coordinates_)
      : coordinates(coordinates_) {};

  auto operator()(const auto &state) const {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(state)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto operator()(const auto &state, double t) const {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(state, t)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto operator()(double t) const {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates)(t)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto prev(const auto &state) const {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::array<double, sizeof...(Coordinates)>{
          std::get<Is>(coordinates).prev(state)...};
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
  auto get_events() {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::tuple_cat(
          std::get<Is>(coordinates).template get_events<Is>()...);
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
};

template <IsSymbol L, IsSymbol R> auto operator|(L l, R r) {
  return Vector(l, r);
}

template <IsSymbol... L, IsSymbol R> auto operator|(Vector<L...> l, R r) {
  return Vector(std::tuple_cat(l.coordinates, std::make_tuple(r)));
}

template <IsSymbol L, IsNotSymbol R> auto operator|(L l, R r) {
  return l | Constant<R>(r);
}

template <size_t derivative = 1, IsSymbol... Coordinates>
constexpr auto D(const Vector<Coordinates...> &vector) {
  if constexpr (derivative == 0) {
    return vector;
  } else {
    return std::apply(
        [&](const auto &...coordinates) { return Vector(D(coordinates)...); },
        vector.coordinates);
  }
}

template <IsSymbol L, IsSymbol R> auto operator&(L l, R r) {
  return std::make_tuple(l, r);
}

template <IsSymbol... L, IsSymbol R> auto operator&(std::tuple<L...> l, R r) {
  return std::tuple_cat(l, std::make_tuple(r));
}

} // namespace diffurch
