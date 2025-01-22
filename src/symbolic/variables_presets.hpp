#pragma once
#include "variables.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

namespace State {
#define TIME_VARIABLE(t) static constexpr auto t = State::TimeVariable();

#define VARIABLE(i, X) static constexpr auto X = State::Variable<i>();

#define VARIABLE_i(a1, i, X) VARIABLE(i, BOOST_PP_CAT(X, i))
#define VARIABLE_2(a1, a2, i, X) VARIABLE(i, X)

#define VARIABLES(...)                                                         \
  BOOST_PP_SEQ_FOR_EACH_I(VARIABLE_2, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define VARIABLES_INDEXED(X, n) BOOST_PP_REPEAT(n, VARIABLE_i, X)

#define VARIABLES_D(X, n) BOOST_PP_REPEAT(n, VARIABLE_D, X)
} // namespace State

namespace Variables_xi_t {
static constexpr auto t = State::TimeVariable();
VARIABLES_INDEXED(x, 10);
} // namespace Variables_xi_t

namespace Variables_x_t {
static constexpr auto t = State::TimeVariable();
static constexpr auto x = State::Variable<0>();
static constexpr auto Dx = State::Variable<1>();
static constexpr auto DDx = State::Variable<2>();
static constexpr auto DDDx = State::Variable<3>();
static constexpr auto DDDDx = State::Variable<4>();
} // namespace Variables_x_t

namespace Variables_xy_t {
static constexpr auto t = State::TimeVariable();
static constexpr auto x = State::Variable<0>();
static constexpr auto y = State::Variable<1>();
} // namespace Variables_xy_t

namespace Variables_xyz_t {
static constexpr auto t = State::TimeVariable();
static constexpr auto x = State::Variable<0>();
static constexpr auto y = State::Variable<1>();
static constexpr auto z = State::Variable<2>();
} // namespace Variables_xyz_t

namespace Variables_y_x {
static constexpr auto x = State::TimeVariable();
static constexpr auto y = State::Variable<0>();
static constexpr auto Dy = State::Variable<1>();
static constexpr auto DDy = State::Variable<2>();
static constexpr auto DDDy = State::Variable<3>();
static constexpr auto DDDDy = State::Variable<4>();
} // namespace Variables_y_x

namespace Variables_rho_theta_t {
static constexpr auto t = State::TimeVariable();
static constexpr auto rho = State::Variable<0>();
static constexpr auto theta = State::Variable<1>();
} // namespace Variables_rho_theta_t
