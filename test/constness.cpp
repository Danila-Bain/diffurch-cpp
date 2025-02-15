
#include <concepts>
#include <iostream>

template <typename F> void test_lambda(F f) {
  std::cout << "Testing lambda" << std::endl;
  if constexpr (requires { f(std::declval<int &&>()); }) {
    std::cout << "Lambda accepts rvalue reference (auto&&)\n";
  }
  if constexpr (requires { f(std::declval<int &>()); }) {
    std::cout << "Lambda accepts lvalue reference (auto&)\n";
  }
}

int main() {
  auto lambda1 = [](auto &x) {
    x++;
    std::cout << x + 1 << std::endl;
  };
  auto lambda2 = [](const auto &x) {
    /*x++;*/
    std::cout << x + 1 << std::endl;
  };

  test_lambda(lambda1);
  test_lambda(lambda2);

  int x1 = 1;
  const int x2 = 2;

  lambda1(x1);
  lambda2(x1);
  /*lambda1(x2);*/ // error: cannot increment read-only reference
  lambda2(x2);

  /*lambda1(std::move(x1)); // error: cannot bind non-const lvalue reference to
   * an rvalue*/
  lambda2(std::move(x1));
  /*lambda1(std::move(x2));*/ // error: cannot increment read-only reference
  lambda2(std::move(x2));
}

/*
 bingo

 const auto &x and auto& x lambdas can be differentiated based on whether they
 accept rvalue references or not, because rvalue reference is convertable to


 */
