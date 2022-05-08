set(PROJECT_UNITTEST_NAME   "ItemEditor.Controllers_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mainController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/manageTexturesController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/texturePickerController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/addItemController_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/addWeaponItemController_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE ItemEditor.Controllers theWarrior.Models ${CONAN_LIBS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})