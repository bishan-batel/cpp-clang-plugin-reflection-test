#include <clang/AST/ASTDumperUtils.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/ASTConsumers.h>
#include <preamble.hpp>
#include "ReflectedClass.hpp"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

// NOLINTBEGIN(*const*)
static cl::OptionCategory lobster_category{"lobster options"};

static cl::extrahelp lobster_common_help{CommonOptionsParser::HelpMessage};

static cl::extrahelp lobster_more_help{
  "\nMore help text...\n",
};

// NOLINTEND(*const*)

class ClassFinder : public MatchFinder::MatchCallback {
public:

  void run(const MatchFinder::MatchResult& result) override {
    context = result.Context;
    source_manager = result.SourceManager;

    const CXXRecordDecl* record{
      result.Nodes.getNodeAs<clang::CXXRecordDecl>("id")
    };

    if (record) {
      return found_record(record);
    }

    const FieldDecl* field{result.Nodes.getNodeAs<FieldDecl>("id")};

    if (field) {
      return found_field(field);
    }

    const FunctionDecl* function{result.Nodes.getNodeAs<CXXMethodDecl>("id")};

    if (function) {
      return found_function(function);
    }
  }

  void onStartOfTranslationUnit() override {}

  void onEndOfTranslationUnit() override {
    std::error_code ec;
    raw_fd_ostream os{filename, ec};

    assert(!ec && "error opening file");

    for (ReflectedClass& ref: classes) {
      ref.generate(context, input_filename, os);
    }
  }

  void found_record(const CXXRecordDecl* record) {
    input_filename = source_manager->getFilename(record->getLocation());

    filename = input_filename;
    filename.erase(filename.end() - 4, filename.end());
    filename.append(".generated.hxx");

    classes.emplace_back(record);
  }

  void found_field(const FieldDecl* field) { classes.back().add_field(field); }

  void found_function(const FunctionDecl* function) {
    assert(function);
    classes.back().add_function(function);
  }

protected:

  ASTContext* context{nullptr};
  SourceManager* source_manager{nullptr};
  Vec<ReflectedClass> classes{};
  String filename{}, input_filename{};
};

class DumpASTAction : public ASTFrontendAction {

  std::unique_ptr<ASTConsumer> CreateASTConsumer(
    CompilerInstance& ci,
    StringRef file
  ) override {
    return CreateASTDumper(
      nullptr,
      "",
      false,
      true,
      true,
      true,
      ASTDumpOutputFormat::ADOF_Default
    );
  }
};

auto main(i32 argc, const char** const argv) -> i32 {
  llvm::Expected<CommonOptionsParser> expected_parser{
    CommonOptionsParser::create(argc, argv, lobster_category)
  };

  if (!expected_parser) {
    llvm::errs() << expected_parser.takeError();
    return 1;
  }

  CommonOptionsParser& options_parser = expected_parser.get();

  ClangTool Tool(
    options_parser.getCompilations(),
    options_parser.getSourcePathList()
  );

  ClassFinder class_finder{};
  MatchFinder finder{};

  static const DeclarationMatcher classMatcher =
    cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
  finder.addMatcher(classMatcher, &class_finder);

  /* Search for all fields with an 'annotate' attribute. */
  static const DeclarationMatcher propertyMatcher =
    fieldDecl(decl().bind("id"), hasAttr(attr::Annotate));
  finder.addMatcher(propertyMatcher, &class_finder);

  /* Search for all functions with an 'annotate' attribute. */
  static const DeclarationMatcher functionMatcher =
    functionDecl(decl().bind("id"), hasAttr(attr::Annotate));
  finder.addMatcher(functionMatcher, &class_finder);

  return Tool.run(newFrontendActionFactory(&finder).get());
}
