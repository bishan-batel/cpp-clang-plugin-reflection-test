add_library(lobster-core
   lobster.cpp
)

target_compile_features(lobster-core PUBLIC cxx_std_23)

target_compile_definitions(
        lobster-core PRIVATE "_DEBUG=$<IF:$<CONFIG:Debug>,1,0>"
        "_RELEASE=$<IF:$<CONFIG:Release>,0,1>"
)

# Add glad and stb_image include directories to lobster target
target_include_directories(
        lobster-core
        PUBLIC ./
        ${crab_SOURCE_DIR}/include
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)
