set(PROJECT_UNITTEST_NAME   "commonEditor.UI_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

find_package(GTest REQUIRED)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterUtils_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/textureUtils_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE commonEditor.UI theWarrior.Models gtest fmt)
gtest_discover_tests(${PROJECT_UNITTEST_NAME})
