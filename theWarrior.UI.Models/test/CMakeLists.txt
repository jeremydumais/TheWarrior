set(PROJECT_UNITTEST_NAME   "theWarrior.UI.Models_unittests")

project(${PROJECT_UNITTEST_NAME} LANGUAGES CXX)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(${PROJECT_UNITTEST_NAME} ${INCLUDE_PATH}
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/message_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/itemFoundMessage_unittest.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/messagePipeline_unittest.cpp)

target_link_libraries(${PROJECT_UNITTEST_NAME} PRIVATE theWarrior.UI.Models ${CONAN_LIBS})
gtest_discover_tests(${PROJECT_UNITTEST_NAME})