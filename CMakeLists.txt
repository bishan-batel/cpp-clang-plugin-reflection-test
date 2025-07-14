cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(na-engine VERSION 1.0)

set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

option(USE_SCCACHE "Use sccache if available" ON)

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

# General options for CMAKE files n output
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# Dependencies

# CPM
include(cmake/CPM.cmake)


# stb configuration
set(stb_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libraries/stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# Add the crab library as a submodule
CPMAddPackage("gh:bishan-batel/crab#reference-type-support")

add_subdirectory(${PROJECT_SOURCE_DIR}/libraries/tiny-process-library)

# Add the glfw library
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "GLFW lib only")

# Add the GLFW source directory
add_subdirectory(${PROJECT_SOURCE_DIR}/libraries/glfw)

# Add the glad source files
add_library(glad STATIC ${PROJECT_SOURCE_DIR}/libraries/glad/src/glad.c)

# Add glad include directory to glad target
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/libraries/glad/include)

# Disable specific warning for glad
target_compile_options(glad PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-language-extension-token>
)

# FMOD
set(FMOD_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libraries/FMOD)
set(FMOD_INCLUDE_DIRS ${FMOD_SOURCE_DIR}/inc)

option(
  FMOD_LOGGING
  "Enable use of libfmodL instead of standard libfmod for logging (slows down program significantly)"
  OFF
)

# fmod logging debug binaries
if(FMOD_LOGGING)
  set(FMOD_LOGGING_SUFFIX "L")
else()
  set(FMOD_LOGGING_SUFFIX "")
endif()

add_library(fmod SHARED IMPORTED GLOBAL)

# Add the fmod library
if(UNIX)
  set_property(TARGET fmod PROPERTY
          IMPORTED_LOCATION "${FMOD_SOURCE_DIR}/libs/linux/x86_64/libfmod${FMOD_LOGGING_SUFFIX}.so.12.10")
elseif(WIN32)
  set_property(TARGET fmod PROPERTY
          IMPORTED_LOCATION "${FMOD_SOURCE_DIR}/libs/windows/x64/fmod${FMOD_LOGGING_SUFFIX}.dll")
  set_property(TARGET fmod PROPERTY
          IMPORTED_IMPLIB "${FMOD_SOURCE_DIR}/libs/windows/x64/fmod${FMOD_LOGGING_SUFFIX}_vc.lib")
endif()

set_target_properties(fmod PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${FMOD_INCLUDE_DIRS}
  INTERFACE_COMPILE_DEFINITIONS "ENABLE_FMOD"
)

# imgui support
add_library(imgui STATIC
  ${PROJECT_SOURCE_DIR}/libraries/imgui/imgui.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/imgui_demo.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/imgui_tables.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_opengl3.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/backends/imgui_impl_glfw.cpp
  ${PROJECT_SOURCE_DIR}/libraries/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
  ${PROJECT_SOURCE_DIR}/libraries/imgui
  ${PROJECT_SOURCE_DIR}/libraries/imgui/backends
  ${PROJECT_SOURCE_DIR}/libraries/imgui/misc/cpp)

target_link_libraries(imgui glfw OpenGL::GL)

# Lua configuration
set(lua_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libraries/lua)
FILE(GLOB lua_sources ${lua_SOURCE_DIR}/*.c)
list(REMOVE_ITEM lua_sources "${lua_SOURCE_DIR}/lua.c" "${lua_SOURCE_DIR}/luac.c")
add_library(lua STATIC ${lua_sources})
target_include_directories(lua PUBLIC $<BUILD_INTERFACE:${lua_SOURCE_DIR}>)

# Sol2 configuration
set(sol2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libraries/sol2)
add_library(sol2_single INTERFACE IMPORTED)
target_include_directories(sol2_single INTERFACE ${sol2_SOURCE_DIR}/include)
target_link_libraries(sol2_single INTERFACE lua)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

option(DO_COPY_ASSETS "Copy assets to bin dir" ON)

# All Subprojects

add_subdirectory(engine)
# add_subdirectory(engine-testing)
add_subdirectory(editor)
add_subdirectory(game)
