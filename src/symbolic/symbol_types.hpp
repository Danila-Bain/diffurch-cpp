#pragma once

#include <type_traits>

namespace diffurch {

#define DECLARE_STATE_EXPRESSION_TYPE(NAME)                                    \
  struct NAME {};                                                              \
                                                                               \
  template <typename T>                                                        \
  concept Is##NAME = std::is_base_of_v<NAME, std::decay_t<T>>;                 \
                                                                               \
  template <typename T>                                                        \
  concept IsNot##NAME = !std::is_base_of_v<NAME, std::decay_t<T>>;

DECLARE_STATE_EXPRESSION_TYPE(Symbol);
DECLARE_STATE_EXPRESSION_TYPE(BoolSymbol);
DECLARE_STATE_EXPRESSION_TYPE(DetectSymbol);
DECLARE_STATE_EXPRESSION_TYPE(SetSymbol);

} // namespace diffurch
