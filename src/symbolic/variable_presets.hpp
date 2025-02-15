#pragma once
#include "variables.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

namespace diffurch {
#define TIME_VARIABLE(t) static constexpr auto t = TimeVariable();

#define VARIABLE(i, X) static constexpr auto X = Variable<i>();

#define VARIABLE_i(a1, i, X) VARIABLE(i, BOOST_PP_CAT(X, i))
#define VARIABLE_2(a1, a2, i, X) VARIABLE(i, X)

#define VARIABLES(...)                                                         \
  BOOST_PP_SEQ_FOR_EACH_I(VARIABLE_2, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define VARIABLES_INDEXED(X, n) BOOST_PP_REPEAT(n, VARIABLE_i, X)

#define VARIABLES_D(X, n) BOOST_PP_REPEAT(n, VARIABLE_D, X)

namespace variables_xi_t {
static constexpr auto t = TimeVariable();
VARIABLES_INDEXED(x, 10);
} // namespace variables_xi_t

namespace variables_x_t {
static constexpr auto t = TimeVariable();
static constexpr auto x = Variable<0>();
static constexpr auto Dx = Variable<1>();
static constexpr auto DDx = Variable<2>();
static constexpr auto DDDx = Variable<3>();
static constexpr auto DDDDx = Variable<4>();
} // namespace variables_x_t

namespace variables_xy_t {
static constexpr auto t = TimeVariable();
static constexpr auto x = Variable<0>();
static constexpr auto y = Variable<1>();
} // namespace variables_xy_t

namespace variables_xyz_t {
static constexpr auto t = TimeVariable();
static constexpr auto x = Variable<0>();
static constexpr auto y = Variable<1>();
static constexpr auto z = Variable<2>();
} // namespace variables_xyz_t

namespace variables_y_x {
static constexpr auto x = TimeVariable();
static constexpr auto y = Variable<0>();
static constexpr auto Dy = Variable<1>();
static constexpr auto DDy = Variable<2>();
static constexpr auto DDDy = Variable<3>();
static constexpr auto DDDDy = Variable<4>();
} // namespace variables_y_x

namespace variables_rho_theta_t {
static constexpr auto t = TimeVariable();
static constexpr auto rho = Variable<0>();
static constexpr auto theta = Variable<1>();
} // namespace variables_rho_theta_t

} // namespace diffurch
