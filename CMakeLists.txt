cmake_minimum_required(VERSION 3.5)

project(pulogulamu VERSION 1.0
               DESCRIPTION "a monadic parser combinator libary"
               LANGUAGES CXX)

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# options
# option(SIMPOU_BUILD_FZZER "Build fuzzer tess.")
# option(SIMPOU_BUILD_TEST "Build unit tests")
# option(SIMPOU_BUILD_DEMO "Build demo programs")
option(SIMPOU_TEST_PARSER "Build demo programs")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# header only lib
include(CMakePackageConfigHelpers)

# target_include_directories(simplou INTERFACE
#   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/simplou>)

add_executable(pulogulamu
  pulogulamu/ast.h
  pulogulamu/lexer.h
  pulogulamu/parser.h
  pulogulamu/parser.cc
  pulogulamu/error.h
  pulogulamu/codegen.h
  pulogulamu/codegen.cc
  )

set_target_properties(pulogulamu PROPERTIES
  C_VISIBILITY_PRESET       hidden
  CXX_VISIBILITY_PRESET     hidden
  VISIBILITY_INLINES_HIDDEN YES
  )

if (TML_BUILD_FUZZER AND TML_BUILD_TEST)
  message(FATAL_ERROR
    "Can't build fuzzer and test together. Please choose one.")
endif()

# test targets
if(SIMPOU_BUILD_FUZZER)
  message("simplou: Building fuzz tests")
  add_subdirectory(fuzzer)
endif()

if (SIMPOU_BUILD_TEST)
  message("simplou: Building unit tests")
  add_subdirectory(test)
endif()

if (SIMPOU_BUILD_DEMO)
  message("simplou: Building demo")
  add_subdirectory(demo)
endif()


if (SIMPOU_TEST_PARSER)
  message("simplou: Building demo")
  add_compile_definitions(SIMPOU_TEST_PARSER)
endif()
