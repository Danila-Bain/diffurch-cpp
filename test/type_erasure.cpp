

#include <iostream>
#include <memory>
struct S {
  int n = 0;

  int get_n_modulo(int k) { return n % k; }
};

using namespace std;

int main(int argc, char *argv[]) {

  S obj1{.n = 11};

  int (S::*ptr)(int) = &S::get_n_modulo;

  auto erased = reinterpret_cast<int (*)(void *, int)>(ptr);

  cout << obj1.get_n_modulo(5) << endl;
  cout << erased(&obj1, 6) << endl;

  return 0;
}
