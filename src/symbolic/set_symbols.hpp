#pragma once

#include "operators.hpp"
#include "symbol_types.hpp"

#include "../util/find_root.hpp"
#include <cstddef>
#include <limits>
#include <tuple>

namespace diffurch {

template <size_t coordinate, IsSymbol R> struct EventSetVariable : SetSymbol {
  R r;
  EventSetVariable(const Variable<coordinate, 0> &var, R r_) : r(r_) {};

  void operator()(auto &state) { state.x_curr[coordinate] = r.prev(state); }
};

template <size_t coordinate, IsSymbol R>
auto operator<<(const Variable<coordinate, 0> &var, const R &r) {
  return EventSetVariable(var, r);
}
template <size_t coordinate, IsNotSymbol R>
auto operator<<(const Variable<coordinate, 0> &var, const R &r) {
  return EventSetVariable(var, Constant(r));
}

template <IsSetSymbol... SetExpr> struct SetMultiple : SetSymbol {
  std::tuple<SetExpr...> set_expr_tuple;
  SetMultiple(const SetExpr &...set_exprs)
      : set_expr_tuple(std::make_tuple(set_exprs...)) {};
  SetMultiple(const std::tuple<SetExpr...> &t_) : set_expr_tuple(t_) {};

  void operator()(auto &state) {
    [this, &state]<size_t... Is>(std::index_sequence<Is...>) {
      (std::get<Is>(set_expr_tuple)(state), ...);
    }(std::make_index_sequence<sizeof...(SetExpr)>{});
  }
};

#define OVERLOAD_SET_MULTIPLE(op)                                              \
  template <IsSetSymbol R, IsSetSymbol L>                                      \
  auto operator op(const R &r, const L &l) {                                   \
    return SetMultiple(r, l);                                                  \
  }                                                                            \
                                                                               \
  template <IsSetSymbol... R, IsSetSymbol L>                                   \
  auto operator op(const SetMultiple<R...> &r, const L &l) {                   \
    return SetMultiple(std::tuple_cat(r.set_expr_tuple, std::make_tuple(l)));  \
  }

OVERLOAD_SET_MULTIPLE(&);
OVERLOAD_SET_MULTIPLE(&&);
OVERLOAD_SET_MULTIPLE(|);
OVERLOAD_SET_MULTIPLE(||);

} // namespace diffurch
