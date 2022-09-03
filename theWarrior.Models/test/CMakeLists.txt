set(PROJECT_UNITTEST_NAME   "theWarrior.Models_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gameMap_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/texture_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/textureContainer_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTile_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/point_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/size_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTrigger_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerEventConverter_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerConditionConverter_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapTileTriggerActionConverter_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/itemStore_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/itemType_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/item_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/armorItem_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/statsItem_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/weaponItem_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/player_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/playerEquipment_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inventory_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monster_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterStore_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/monsterZone_unittest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rgbItemColor_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE theWarrior.Models ${CONAN_LIBS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})
