cmake_minimum_required(VERSION 3.5)

project(pulogulamu VERSION 1.0
               DESCRIPTION "a monadic parser combinator libary"
               LANGUAGES CXX)

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# options
# option(PULOGULAMU_BUILD_FZZER "Build fuzzer tess.")
# option(PULOGULAMU_BUILD_TEST "Build unit tests")
# option(PULOGULAMU_BUILD_DEMO "Build demo programs")
# option(PULOGULAMU_TEST_PARSER "Build demo programs")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# header only lib
include(CMakePackageConfigHelpers)

# target_include_directories(simplou INTERFACE
#   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/simplou>)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig in ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(pulogulamu
  pulogulamu/main.cc
  pulogulamu/ast.h
  pulogulamu/lexer.h
  pulogulamu/parser.h
  pulogulamu/parser.cc
  pulogulamu/error.h
  pulogulamu/codegen.h
  pulogulamu/codegen.cc
  )

target_link_libraries(pulogulamu ${llvm_libs})
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
if(PULOGULAMU_BUILD_FUZZER)
  message("simplou: Building fuzz tests")
  add_subdirectory(fuzzer)
endif()

if (PULOGULAMU_BUILD_TEST)
  message("simplou: Building unit tests")
  add_subdirectory(test)
endif()

if (PULOGULAMU_BUILD_DEMO)
  message("simplou: Building demo")
  add_subdirectory(demo)
endif()


if (PULOGULAMU_TEST_PARSER)
  message("simplou: Building demo")
  add_compile_definitions(PULOGULAMU_TEST_PARSER)
endif()
