cmake_minimum_required(VERSION 3.20)
project(LICMPass CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include(HandleLLVMOptions)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(LICMPass MODULE src/LICMPass.cpp)
set_target_properties(LICMPass PROPERTIES PREFIX "")
if(NOT APPLE)
  set_target_properties(LICMPass PROPERTIES LINK_FLAGS "-Wl,-znodelete")
endif()

add_custom_target(run-test
  DEPENDS LICMPass
  COMMAND clang -O0 -S -emit-llvm ${CMAKE_SOURCE_DIR}/test/sample.c -o ${CMAKE_BINARY_DIR}/sample.ll
  COMMAND opt -load-pass-plugin $<TARGET_FILE:LICMPass> -passes=licm-custom -S ${CMAKE_BINARY_DIR}/sample.ll -o ${CMAKE_BINARY_DIR}/sample_opt.ll
  COMMAND diff ${CMAKE_BINARY_DIR}/sample.ll ${CMAKE_BINARY_DIR}/sample_opt.ll || true
  COMMENT "Compiling sample.c to IR, running LICM pass, diffing result"
  VERBATIM
)
