#pragma once

#include <preamble.hpp>
#include <option.hpp>
#include <box.hpp>
#include <ranges>
#include <rc.hpp>
#include <typeindex>

#define NA_OBJECT(...) __attribute__((annotate("na_object;", #__VA_ARGS__)))

#define NA_METHOD(...)                                                         \
  __attribute__((annotate("na_bind_method;", #__VA_ARGS__)))

#define NA_PROPERTY(...)                                                       \
  __attribute__((annotate("na_bound_field;", #__VA_ARGS__)))

#define auto auto

class Object {
  ~Object() = default;

public:

  Object(const Object&) = default;

  Object(Object&&) = delete;

  auto operator=(const Object&) -> Object& = default;

  auto operator=(Object&&) -> Object& = delete;
};

namespace lobster {

  struct Type;

  struct Type final {

    enum Category {
      C_INTEGER,
      C_DECIMAL,
      C_STRING,
      C_BOOL,
      C_OBJECT,
      C_NIL
    };

    [[nodiscard]] auto get_type() const -> Category;

    [[nodiscard]] auto is_object() const -> bool;

    [[nodiscard]] auto is_array() const -> bool;

    [[nodiscard]] auto is_integer() const -> bool;

    [[nodiscard]] auto is_decimal() const -> bool;

    [[nodiscard]] auto is_number() const -> bool;

    [[nodiscard]] auto is_bool() const -> bool;

    [[nodiscard]] static auto boolean() -> Type;

    [[nodiscard]] static auto integer() -> Type;

    [[nodiscard]] static auto decimal() -> Type;

    [[nodiscard]] static auto string() -> Type;

    [[nodiscard]] static auto nil() -> Type;

    [[nodiscard]] static auto object(String name) -> Type;

    [[nodiscard]] auto optional() const& -> Type;

    [[nodiscard]] auto optional() && -> Type;

    [[nodiscard]] auto to_string() const -> String;

  private:

    explicit Type(Category category, Option<String> name = {}):
        category{category}, name{std::move(name)} {}

    Category category;
    Option<String> name;
    bool is_optional{false};
  };

  struct Method final {
    String name{};
    Vec<Type> arg_types{};
    Vec<String> arg_names{};
    Type return_type;
  };

  struct ClassMeta final {
    String name;
    std::type_index type;
    Option<String> base{};
    Vec<Method> methods;
  };

  class Registry {

  public:

    Registry() = delete;

    [[nodiscard]]
    static auto get(const String& name) -> Option<const ClassMeta&>;

    template<std::derived_from<Object> T>
    [[nodiscard]] static auto get() -> const ClassMeta& {
      return classes.at(typeid(T));
    }

    template<std::derived_from<Object> T>
    static auto _register_class(ClassMeta meta) -> void {
      Rc<ClassMeta> metadata{crab::make_rc<ClassMeta>(std::move(meta))};

      classes.emplace(typeid(T), metadata);
      name_lookup.emplace(metadata->name, metadata);
    }

    template<typename T>
    struct AddClass final {
      AddClass(String name, String base, Vec<Method> methods) {
        _register_class<T>(
          {std::move(name), typeid(T), std::move(base), methods}
        );
      }
    };

    static auto all_classes() {
      const Dictionary<std::type_index, Rc<ClassMeta>>& classes{
        Registry::classes
      };

      return classes | views::values;
    }

  private:

    inline static Dictionary<std::type_index, Rc<ClassMeta>> classes{};
    inline static Dictionary<String, Rc<ClassMeta>> name_lookup{};
  };
}
