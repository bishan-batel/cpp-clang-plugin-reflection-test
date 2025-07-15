cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(refl-test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

option(USE_SCCACHE "Use SCCACHE" ON)

if (${USE_SCCACHE})
  find_program(SCCACHE sccache)

  if (EXISTS ${SCCACHE})
    message("Using SCCACHE ${SCCACHE}")
    set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE})
  else()
    message("Failed to find SCCACHE")
  endif()

endif()

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# CPM
include(cmake/CPM.cmake)

CPMAddPackage("gh:bishan-batel/crab#reference-type-support")

add_subdirectory(lobster)
add_subdirectory(plugin)
add_subdirectory(test)
