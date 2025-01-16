#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#define PP_state_lambda_variable_evaluation_lambda(r, data, index, var)        \
  template <size_t derivative_order = 0>                                       \
  auto BOOST_PP_CAT(var, _) =                                                  \
      [&, this](double t) { return state.eval<derivative_order, index>(t); };

#define PP_state_lambda_return_part(ret)                                       \
  return ret;                                                                  \
  }

#define PP_state_lambda_declarations_part(...)                                 \
  (auto &state) {                                                              \
    auto &t = state.curr_t;                                                    \
    auto &[__VA_ARGS__] = state.curr_x;                                        \
    BOOST_PP_SEQ_FOR_EACH_I(PP_state_lambda_variable_evaluation_lambda, _,     \
                            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __state_lambda__(...)                                                  \
  PP_state_lambda_declarations_part(__VA_ARGS__) PP_state_lambda_return_part

/*call like this: [this]__state_lambda__(x,y,z)(x+y+z)*/
