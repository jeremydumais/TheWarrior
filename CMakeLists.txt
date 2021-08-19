cmake_minimum_required(VERSION 3.10)

project(TheWarrior C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

if (UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" LOWERCASE_CMAKE_BUILD_TYPE)
if ("${LOWERCASE_CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    message("-- Debug mode selected")
elseif(LOWERCASE_CMAKE_BUILD_TYPE MATCHES "^(release)$")
    message("-- Release mode selected")
endif()
set(CMAKE_CXX_STANDARD 14)

include(Conan)
run_conan()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if (${CODE_COVERAGE})
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

enable_testing()
include(GoogleTest)

add_subdirectory("twModels")
add_subdirectory("twUI")
add_subdirectory("theWarrior")
add_subdirectory("mapEditor.Controllers")
add_subdirectory("mapEditor")
add_subdirectory("itemEditor")