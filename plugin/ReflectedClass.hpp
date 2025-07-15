#pragma once

#include <clang/AST/ASTDumperUtils.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/ASTConsumers.h>
#include <iostream>
#include <llvm/ADT/StringRef.h>
#include <preamble.hpp>
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

using namespace clang;
using namespace clang::tooling;

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

class ReflectedClass {
public:

  explicit ReflectedClass(const CXXRecordDecl* rec);

  /* ===--------------------------------------------------=== */
  /* Public API                                               */
  /* ===--------------------------------------------------=== */
  void generate(ASTContext* ctx, String input, raw_ostream& os);

  void add_field(const FieldDecl* field);

  void add_function(const FunctionDecl* function);

private:

  template<class T>
  Attr* get_annotate_attr(T* decl) const;

  template<u32 N>
  StringRef get_annotations(const Attr* attr, SmallString<N>& str) const;

  template<std::invocable<StringRef> F>
  bool for_each_property(const StringRef& annotations, F&& filter);

  bool has_function_of_name(const char* name) const;

  template<class T, u32 N = 32>
  bool has_annotation(T& decl, const char* pattern) const;

private:

  const CXXRecordDecl* record{nullptr};
  Vec<const FieldDecl*> fields{nullptr};
  Vec<const FunctionDecl*> functions{nullptr};
};

template<class T, u32 N>
inline bool ReflectedClass::has_annotation(T& decl, const char* pattern) const {
  const Attr* attribute{get_annotate_attr(&decl)};

  if (attribute == nullptr) {
    return false;
  }

  SmallString<N> string{};
  return get_annotations(attribute, string).starts_with(pattern);
}

template<std::invocable<StringRef> F>
inline bool ReflectedClass::for_each_property(
  const StringRef& annotations,
  F&& filter
) {

  auto [property, rest] = annotations.split(',');

  if (property.size() == 0) {
    return true;
  }
  if (!filter(property.trim())) {
    return false;
  }

  return for_each_property(rest, std::forward<F>(filter));
}

template<u32 N>
inline clang::StringRef ReflectedClass::get_annotations(
  const Attr* attr,
  SmallString<N>& str
) const {
  str.clear();
  raw_svector_ostream os{str};
  LangOptions langopts;
  PrintingPolicy policy{langopts};
  attr->printPretty(os, policy);
  return str.slice(25, str.size() - 4);
}

template<class T>
inline clang::Attr* ReflectedClass::get_annotate_attr(T* decl) const {
  for (auto& attr: decl->attrs()) {
    if (attr->getKind() == attr::Annotate) {
      return attr;
    }
  }

  return nullptr;
}
