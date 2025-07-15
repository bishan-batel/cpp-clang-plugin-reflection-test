
file(GLOB_RECURSE REFLECTION_SOURCES *.hpp)

# list(APPEND GENERATED_SOURCES)

foreach(SOURCE IN ITEMS ${REFLECTION_SOURCES})
  string(REPLACE ".hpp" ".generated.cc" SOURCE ${SOURCE})
  message("Source output to ${SOURCE}")
  list(APPEND GENERATED_SOURCES ${SOURCE})
endforeach()

add_custom_target(generate_reflection ALL DEPENDS ${GENERATED_SOURCES})

add_executable(reflection-test
     main.cpp
     ${GENERATED_SOURCES}
)

add_custom_command(
  OUTPUT ${GENERATED_SOURCES}
  COMMAND ${CMAKE_BINARY_DIR}/plugin/lobster -p ${CMAKE_BINARY_DIR} ${REFLECTION_SOURCES}
  DEPENDS lobster ${REFLECTION_SOURCES}
  COMMENT "Generating reflection files"
)


add_dependencies(reflection-test generate_reflection)


target_compile_features(reflection-test PUBLIC cxx_std_23)

# target_link_libraries(reflection-test PUBLIC # Shared Dependencies)

target_compile_definitions(
    reflection-test PRIVATE "_DEBUG=$<IF:$<CONFIG:Debug>,1,0>"
    "_RELEASE=$<IF:$<CONFIG:Release>,0,1>"
)

target_link_libraries(reflection-test lobster-core)

# Add glad and stb_image include directories to reflection-test target
target_include_directories(
    reflection-test
    PUBLIC ./
    ${crab_SOURCE_DIR}/include
    ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)

# target_precompile_headers(
#   reflection-test
#         PRIVATE
#         <preamble.hpp>
#         <box.hpp>
#         <result.hpp>
#         <option.hpp>
#         <ref.hpp>
#         <rc.hpp>
#         <unordered_map>
#         <unordered_set>
#         <utility>
#         <memory>
#         <cmath>
#         <iostream>
#         <string>
#         <vector>)

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
# add_custom_target(dummy "" DEPENDS lobster)
# add_dependencies(reflection-test dummy)


# vim: shiftwidth=2 tabstop=2:
