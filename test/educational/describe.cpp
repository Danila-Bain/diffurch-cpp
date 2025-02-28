
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <iostream>

using namespace boost::describe;
using namespace boost::mp11;

struct Person {
  std::string name;
  int age;
  double height;

  BOOST_DESCRIBE_CLASS(Person, (), (), (), ()) // No manual member listing!
};

template <typename T> void print_members(const T &obj) {
  mp_for_each<describe_members<T, mod_public>>([&](auto member) {
    std::cout << member.name << " = " << obj.*(member.pointer) << std::endl;
  });
}

int main() {
  Person p{"Alice", 30, 1.75};
  print_members(p);
  return 0;
}
