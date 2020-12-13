macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  conan_add_remote(
    NAME
    bincrafters
    URL
    https://api.bintray.com/conan/bincrafters/public-conan)

set(COMPILER_RUNTIME "")
if (WIN32)
  if (LOWERCASE_CMAKE_BUILD_TYPE MATCHES "^(debug)$" OR LOWERCASE_CMAKE_BUILD_TYPE STREQUAL "")
    set(COMPILER_RUNTIME "compiler.runtime=MDd")
  else()
  set(COMPILER_RUNTIME "compiler.runtime=MD")
  endif()
endif()

  conan_cmake_run(
    REQUIRES
    ${CONAN_EXTRA_REQUIRES}
    boost/1.73.0
    gtest/1.10.0
    fmt/7.0.1
    sqlite3/3.32.3
    SETTINGS ${COMPILER_RUNTIME}
    OPTIONS
    ${CONAN_EXTRA_OPTIONS}
    BASIC_SETUP
    CMAKE_TARGETS # individual targets to link to
    BUILD
    missing)
endmacro()