find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)

find_package(Clang REQUIRED)

include_directories(${CLANG_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(LLVM_LINK_COMPONENTS options support)


add_executable(lobster
  Lobster.cpp
  ReflectedClass.cpp
)

target_include_directories(lobster PRIVATE ${LLVM_INCLUDE_DIRS})
# set_target_properties(lobster PROPERTIES SUFFIX ".so")

target_compile_options(lobster PRIVATE
        # -Wall
        # -Wextra
        # -Wpedantic
        -std=c++23
        # -Wno-missing-field-initializers
        # -pedantic
        # -Wnon-virtual-dtor
        # -Wunused
        # -Woverloaded-virtual
)

target_link_libraries(lobster 
PRIVATE
  lobster-core
  crab
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangSerialization
  clangStaticAnalyzerFrontend
  clangTooling
  clangToolingSyntax
)

