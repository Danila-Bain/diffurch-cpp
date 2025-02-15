#pragma once

#include <type_traits>

namespace State {

#define DECLARE_STATE_EXPRESSION_TYPE(NAME)                                    \
  struct NAME {};                                                              \
                                                                               \
  template <typename T>                                                        \
  concept Is##NAME = std::is_base_of_v<NAME, std::decay_t<T>>;                 \
                                                                               \
  template <typename T>                                                        \
  concept IsNot##NAME = !std::is_base_of_v<NAME, std::decay_t<T>>;

DECLARE_STATE_EXPRESSION_TYPE(StateExpression);
DECLARE_STATE_EXPRESSION_TYPE(StateBoolExpression);
DECLARE_STATE_EXPRESSION_TYPE(StateDetectExpression);
DECLARE_STATE_EXPRESSION_TYPE(StateSetExpression);

} // namespace State
