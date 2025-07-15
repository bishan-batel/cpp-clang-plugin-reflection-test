#include "lobster.hpp"
#include "preamble.hpp"

namespace lobster {

  auto Type::get_type() const -> Category { return category; }

  auto Type::is_object() const -> bool { return category == C_OBJECT; }

  auto Type::integer() -> Type { return Type{C_INTEGER}; };

  auto Type::decimal() -> Type { return Type{C_DECIMAL}; };

  auto Type::string() -> Type { return Type{C_DECIMAL}; };

  auto Type::object(String name) -> Type {
    return Type{C_OBJECT, std::move(name)};
  };

  auto Type::optional() const& -> Type { return Type{*this}.optional(); };

  auto Type::optional() && -> Type {
    is_optional = true;
    return std::move(*this);
  };

  auto Type::is_integer() const -> bool { return category == C_INTEGER; }

  auto Type::is_decimal() const -> bool { return category == C_DECIMAL; }

  auto Type::is_number() const -> bool { return is_integer() or is_decimal(); }

  auto Type::is_bool() const -> bool { return is_bool(); }

  auto Type::boolean() -> Type { return Type{C_BOOL}; }

  auto Registry::get(const String& name) -> Option<const ClassMeta&> {
    return crab::then(name_lookup.contains(name), [&] -> const ClassMeta& {
      return name_lookup.at(name);
    });
  }

  auto Type::nil() -> Type { return Type{C_NIL}; }

  auto Type::to_string() const -> String {
    OutStringStream os{};

    switch (category) {
      case C_INTEGER: os << "integer"; break;
      case C_DECIMAL: os << "decimal"; break;
      case C_STRING: os << "string"; break;
      case C_BOOL: os << "bool"; break;
      case C_NIL: os << "nil"; break;
      case C_OBJECT: os << name.get_unchecked(); break;
    }

    if (is_optional) {
      os << "?";
    }

    return os.str();
  }
}
