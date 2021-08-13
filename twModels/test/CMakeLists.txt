set(PROJECT_UNITTEST_NAME   "twModels_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gameMap_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/texture_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTile_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/point_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTrigger_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerEventConverter_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerConditionConverter_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerActionConverter_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE twModels ${CONAN_LIBS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})