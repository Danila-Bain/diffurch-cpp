#pragma once

#include "expression.hpp"
#include <cstddef> // for size_t
#include <tuple>

namespace diffurch {

template <IsStateExpression... Coordinates> struct Vector : StateExpression {
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
  auto get_events() const {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
      return std::make_tuple(std::get<Is>(coordinates).get_events()...);
    }(std::make_index_sequence<sizeof...(Coordinates)>{});
  }
};

template <IsStateExpression L, IsStateExpression R> auto operator|(L l, R r) {
  return Vector(l, r);
}

template <IsStateExpression... L, IsStateExpression R>
auto operator|(Vector<L...> l, R r) {
  return Vector(std::tuple_cat(l.coordinates, std::make_tuple(r)));
}

template <size_t derivative = 1, IsStateExpression... Coordinates>
constexpr auto D(const Vector<Coordinates...> &vector) {
  if constexpr (derivative == 0) {
    return vector;
  } else {
    return std::apply(
        [&](const auto &...coordinates) { return Vector(D(coordinates)...); },
        vector.coordinates);
  }
}

template <IsStateExpression L, IsStateExpression R> auto operator&(L l, R r) {
  return std::make_tuple(l, r);
}

template <IsStateExpression... L, IsStateExpression R>
auto operator&(std::tuple<L...> l, R r) {
  return std::tuple_cat(l, std::make_tuple(r));
}

} // namespace diffurch
