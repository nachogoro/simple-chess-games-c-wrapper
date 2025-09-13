cmake_minimum_required(VERSION 3.14)
project(simple-chess-games-c-wrapper VERSION 0.0.1 DESCRIPTION "C wrapper for simple-chess-games library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Fetch the C++ library from GitHub
include(FetchContent)
FetchContent_Declare(
  simple-chess-games
  GIT_REPOSITORY https://github.com/nachogoro/simple-chess-games.git
  GIT_TAG main
)
FetchContent_MakeAvailable(simple-chess-games)

# Create directories
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Define source files for the wrapper
set(WRAPPER_SOURCES
    src/simplechess_c.cpp
)

# Define header files for the wrapper
set(WRAPPER_HEADERS
    include/simplechess/simplechess.h
)

# Create shared library
add_library(simplechess-c SHARED ${WRAPPER_SOURCES})
target_include_directories(simplechess-c PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(simplechess-c PRIVATE
    ${simple-chess-games_SOURCE_DIR}/include
)
target_link_libraries(simplechess-c PRIVATE simple-chess-games)
set_target_properties(simplechess-c PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    PUBLIC_HEADER "${WRAPPER_HEADERS}"
)

# Create static library
add_library(simplechess-c-static STATIC ${WRAPPER_SOURCES})
target_include_directories(simplechess-c-static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(simplechess-c-static PRIVATE
    ${simple-chess-games_SOURCE_DIR}/include
)
target_link_libraries(simplechess-c-static PRIVATE simple-chess-games-static)
set_target_properties(simplechess-c-static PROPERTIES
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME simplechess-c
    PUBLIC_HEADER "${WRAPPER_HEADERS}"
)

# Test executable using shared library
add_executable(test_suite tests/test_suite.c)
target_include_directories(test_suite PRIVATE include)
target_link_libraries(test_suite PRIVATE simplechess-c)

# Test executable using static library
add_executable(test_suite_static tests/test_suite.c)
target_include_directories(test_suite_static PRIVATE include)
target_link_libraries(test_suite_static PRIVATE simplechess-c-static)

# Copy outputs to bin directory with shell commands
add_custom_target(copy_to_bin ALL
    COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/bin
    COMMAND cp -P libsimplechess-c.so* ${CMAKE_CURRENT_SOURCE_DIR}/bin/ || true
    COMMAND cp libsimplechess-c.a ${CMAKE_CURRENT_SOURCE_DIR}/bin/ || true
    COMMAND cp test_suite ${CMAKE_CURRENT_SOURCE_DIR}/bin/ || true
    COMMAND cp test_suite_static ${CMAKE_CURRENT_SOURCE_DIR}/bin/ || true
    DEPENDS simplechess-c simplechess-c-static test_suite test_suite_static
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Installation
include(GNUInstallDirs)

install(TARGETS simplechess-c simplechess-c-static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
