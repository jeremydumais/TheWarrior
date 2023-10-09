set(PROJECT_UNITTEST_NAME   "ItemEditor.Controllers_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

find_package(GTest REQUIRED)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mainController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/texturePickerController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageItemController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageArmorItemController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageStatsItemController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageWeaponItemController_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE ItemEditor.Controllers theWarrior.Models gtest)
gtest_discover_tests(${PROJECT_UNITTEST_NAME})
