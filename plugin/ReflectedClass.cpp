#include "ReflectedClass.hpp"
#include <clang/AST/Decl.h>
#include <clang/AST/Type.h>
#include <clang/Basic/Specifiers.h>
#include <functional>
#include <option.hpp>
#include <preamble.hpp>
#include "lobster.hpp"

bool ReflectedClass::has_function_of_name(const char* name) const {
  SmallString<64> str;
  return ranges::any_of(functions, [&str, &name](const FunctionDecl* const& v) {
    str.clear();
    raw_svector_ostream fos{str};
    v->printName(fos);
    return str == name;
  });
}

ReflectedClass::ReflectedClass(const CXXRecordDecl* rec): record{rec} {}

void ReflectedClass::generate(
  ASTContext* ctx,
  String input_file,
  raw_ostream& os
) {

  String base{record->bases().begin()->getType()->getTypeClassName()};

  // for (const clang::CXXBaseSpecifier& base: record->bases()) {
  //   if (base.getAccessSpecifier() != AccessSpecifier::AS_public) {
  //     continue;
  //   }
  //
  //   QualType a = base.getType();
  // }

  OutStringStream methods_streams{};

  auto from_type{[](const QualType& type) -> Option<String> {
    if (type->isIntegerType()) {
      return String{"lobster::Type::integer()"};
    } else if (type->isBooleanType()) {
      return String{"lobster::Type::boolean()"};
    } else if (type->isFloatingType()) {
      return String{"lobster::Type::decimal()"};
    } else if (type->isVoidType()) {
      return String{"lobster::Type::nil()"};
    }

    errs() << "Failed to parse QualType into lobster::Type : \""
           << type.getAsString() << '"';

    return crab::none;
  }};

  for (const FunctionDecl* func: functions) {
    if (not func) {
      continue;
    }

    OutStringStream method{};

    method << "lobster::Method{\"" << func->getName().str() << "\", {";

    bool exit{false};

    for (ParmVarDecl* param: func->parameters()) {
      if (auto ty{from_type(param->getType().getDesugaredType(*ctx))}) {
        method << ty.get_unchecked() << ",";
      } else {
        exit = true;
        break;
      }
    }

    if (exit) {
      continue;
    }
    method << "},{";

    for (ParmVarDecl* param: func->parameters()) {
      method << '"' << param->getName().str() << "\",";
    }

    method << "},";

    if (auto ty = from_type(func->getReturnType().getDesugaredType(*ctx))) {
      method << ty.get_unchecked();
    } else {
      continue;
    }

    method << "},";
    methods_streams << method.str();
  }

  os << std::format(
    R"(
/** 
 * GENERATED FILE, DO NOT EDIT
 */

#include "{3}"
#include <lobster.hpp>

namespace {{ 
  static lobster::Registry::AddClass<{0}> CLASS_REGISTRY_{0}{{ 
    "{0}", 
    "{1}", 
    {{{2}}} 
  }}; 
}}
  )",
    record->getNameAsString(),
    base,
    methods_streams.str(),
    input_file
  );
}

void ReflectedClass::add_field(const FieldDecl* field) {
  if (not has_annotation(*field, "na_bound_field")) {
    return;
  }

  fields.push_back(field);
}

void ReflectedClass::add_function(const FunctionDecl* function) {
  if (not has_annotation(*function, "na_bind_method")) {
    // outs() << "\t> Ignoring function " << function->getNameAsString() <<
    // '\n';
    return;
  }

  functions.push_back(function);
}
