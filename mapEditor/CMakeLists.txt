# Set project name
set(PROJECT_NAME    "MapEditor")

include_directories(${QT_INCLUDE_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/../commonUI/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Models/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Storage/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../mapEditor.Controllers/include)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

#Generate QT Form headers
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS AUTOGEN_BUILD_DIR/include)

set (RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../commonUI/res/darkstyle/darkstyle.qss)

set(PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/mainForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/components/mainForm_GLComponent.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/components/mainForm_TileTabComponent.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/components/mainForm_TextureListTabComponent.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/components/mainForm_TextureSelectionComponent.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/components/mainForm_MapTabComponent.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editTextureForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editMapTileTriggerForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editTileActionChangeMapPropertiesForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editTileActionOpenChestPropertiesForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/mapOpenGLWidget.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/qClickableLabel.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/aboutBoxForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/imageAttributionsForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/../commonUI/res/darkstyle/darkstyle.qrc
                         ${CMAKE_CURRENT_SOURCE_DIR}/res/global.qrc)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_definitions(${PROJECT_NAME} PRIVATE SUPPORT_MAYBE_UNUSED)
endif()

target_link_libraries(${PROJECT_NAME} MapEditor.Controllers theWarrior.Models theWarrior.Storage commonUI Qt5::Widgets Qt5::OpenGL glut GLU GL ${CONAN_LIBS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") #gcc
  # https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
  target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wfatal-errors" # stops on first error
    "-Wall" # enables all the warnings about constructions
    "-Wextra" # Print extra warning messages"
    "-Werror=misleading-indentation" # Make all warnings into errors.
    "-pedantic" # Issue all the warnings demanded by strict ISO C and ISO C++
    "-pedantic-errors" # Like -pedantic, except that errors are produced rather than warnings http://stackoverflow.com/questions/30255294/how-to-hide-extra-output-from-pragma-message
    "-ftrack-macro-expansion=0"
    "-fno-diagnostics-show-caret"
    "-Wno-misleading-indentation"
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wfatal-errors" # stops on first error
    "-Wall" # enables all the warnings about constructions
    "-Wextra" # Print extra warning messages"
    "-Werror" # Make all warnings into errors.
    "-Wno-unused-lambda-capture" # FIX: It is necessary when building on MSVC
    "-pedantic" # Issue all the warnings demanded by strict ISO C and ISO C++
    "-pedantic-errors" # Like -pedantic, except that errors are produced rather than warnings http://stackoverflow.com/questions/30255294/how-to-hide-extra-output-from-pragma-message
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${PROJECT_NAME}
      PUBLIC /wdC4251
      INTERFACE NOMINMAX # avoid Win macro definition of min/max, use std one
      INTERFACE _SCL_SECURE_NO_WARNINGS) # disable security-paranoia warning
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${RESOURCE_FILES} DESTINATION share/${PROJECT_NAME}/res)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res  DESTINATION ${CMAKE_BINARY_DIR}/bin)
