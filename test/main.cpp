#include <iostream>
#include <print>
#include "lobster.hpp"

#include "TestClass.hpp"
#include "TestClass.generated.hxx"

int main() {
  std::cout
    << lobster::Registry::get("TestClass").map(&lobster::ClassMeta::base)
    << std::endl;

  for (const lobster::ClassMeta& meta: lobster::Registry::all_classes()) {
    std::cout << "> " << meta.name << std::endl;

    for (const lobster::Method& method: meta.methods) {
      std::cout << "Name: " << method.name << std::endl;
    }
  }

  return 0;
}
