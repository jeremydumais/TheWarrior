set(PROJECT_UNITTEST_NAME   "MapEditor.Controllers_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

find_package(GTest REQUIRED)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/containerOfMonsterStoreTestUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/editMapTileTriggerFormController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/editMonsterEncounterFormController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/editMonsterZoneFormController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/itemStoreInfoJSONSerializer_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mainController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageItemStoreController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageMonsterStoreController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileDTOUtils_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterPickerFormController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterStoreInfoJSONSerializer_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterZoneDTOUtils_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/glComponentController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tilePropsComponentController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/toolbarStateInfoJSONSerializer_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE MapEditor.Controllers theWarrior.Models gtest)
gtest_discover_tests(${PROJECT_UNITTEST_NAME})
