#include <iostream>
#include <ostream>
#include <type_traits>
#include <utility>

template <typename T> class Constant {
public:
  // For lvalue arguments: store by value
  template <typename U>
    requires(!std::is_rvalue_reference<U &&>::value)
  explicit Constant(U &&val) : value(std::forward<U>(val)) {}

  // For rvalue arguments: store by reference
  template <typename U>
    requires(std::is_rvalue_reference<U &&>::value)
  explicit Constant(U &&val) : value(val) {}

  // The value member
  std::conditional_t<std::is_rvalue_reference<T &&>::value, T &, T> value;
};

// Helper type deduction guide (C++17 and later)
template <typename T> Constant(T &&) -> Constant<T>;

int main() {

  int orig = 1;
  auto a = Constant(orig - 1);
  std::cout << a.value << std::endl;
  orig = 2;
  std::cout << a.value << std::endl;

  /*std::cout << "1" << std::endl;*/

  return 0;
}
