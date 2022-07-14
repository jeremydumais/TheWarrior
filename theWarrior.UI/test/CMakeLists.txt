set(PROJECT_UNITTEST_NAME   "theWarrior.UI_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/glColor_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/inputDevicesState_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE theWarrior.UI SDL2 ${CONAN_LIBS})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CONAN_INCLUDE_DIRS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})