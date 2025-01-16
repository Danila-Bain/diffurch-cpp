
template <typename RK>
concept RK_Table = requires {
  RK::s; // order of rk method
  RK::A;
  RK::B;
  RK::C;
};
