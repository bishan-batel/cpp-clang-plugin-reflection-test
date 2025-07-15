#pragma once

#include "lobster.hpp"

class NA_OBJECT() TestClass : public Object {

public:

  NA_METHOD() auto bruh() -> void {}

  /**
   * @brief Sets 'what' property
   */
  NA_METHOD() auto set_what(i32 x) -> void { what = x; }

  /**
   * @brief Gets 'what' property
   */
  [[nodiscard]]
  NA_METHOD() auto get_what() const -> i32 {
    return what;
  }

private:

  NA_PROPERTY(setter = set_what, getter = get_what, default = 0)
  i32 what;
};

class NA_OBJECT() TestDerived : public Object {
public:

  NA_METHOD() auto say_hi() -> void { std::cout << "hi" << std::endl; }
};
