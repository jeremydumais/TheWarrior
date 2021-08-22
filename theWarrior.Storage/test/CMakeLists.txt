set(PROJECT_UNITTEST_NAME   "theWarrior.Storage_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../theWarrior.Models/include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gameMapStorage_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE theWarrior.Storage ${CONAN_LIBS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})