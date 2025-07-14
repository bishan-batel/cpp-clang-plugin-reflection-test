add_executable(reflection-test
   main.cpp
)

target_compile_features(reflection-test PUBLIC cxx_std_23)

# target_link_libraries(reflection-test PUBLIC # Shared Dependencies)

target_compile_definitions(
        reflection-test PRIVATE "_DEBUG=$<IF:$<CONFIG:Debug>,1,0>"
        "_RELEASE=$<IF:$<CONFIG:Release>,0,1>"
)

# Add glad and stb_image include directories to reflection-test target
target_include_directories(
        reflection-test
        PUBLIC ./
        ${crab_SOURCE_DIR}/include
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)

target_precompile_headers(
  reflection-test
        PRIVATE
        <preamble.hpp>
        <box.hpp>
        <result.hpp>
        <option.hpp>
        <ref.hpp>
        <rc.hpp>
        <unordered_map>
        <unordered_set>
        <utility>
        <memory>
        <cmath>
        <iostream>
        <string>
        <vector>)

# --------------------------------
# Compile Flags
# --------------------------------

target_compile_options(reflection-test PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -std=c++23
        -Wno-missing-field-initializers
        -pedantic
        -Wnon-virtual-dtor
        -Wunused
        -Woverloaded-virtual
        # -ftime-trace
)
