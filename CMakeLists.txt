cmake_minimum_required(VERSION 3.10)

project(TheWarrior C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

if (UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" LOWERCASE_CMAKE_BUILD_TYPE)
if ("${LOWERCASE_CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    message("-- Debug mode selected")
elseif(LOWERCASE_CMAKE_BUILD_TYPE MATCHES "^(release)$")
    message("-- Release mode selected")
endif()
set(CMAKE_CXX_STANDARD 20)

# Fix to consider Conan libraries as system and ignore compiler flags
include_directories(SYSTEM ${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

if (${CODE_COVERAGE})
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

enable_testing()
include(GoogleTest)

add_subdirectory("theWarrior.Models")
add_subdirectory("theWarrior.Storage")
add_subdirectory("theWarrior.UI.Models")
add_subdirectory("theWarrior.UI.Controllers")
add_subdirectory("theWarrior.UI")
add_subdirectory("theWarrior.Utils")
add_subdirectory("theWarrior")
add_subdirectory("commonEditor.UI")
add_subdirectory("mapEditor.Controllers")
add_subdirectory("mapEditor")
add_subdirectory("itemEditor.Controllers")
add_subdirectory("itemEditor")
add_subdirectory("monsterEditor.Controllers")
add_subdirectory("monsterEditor")
