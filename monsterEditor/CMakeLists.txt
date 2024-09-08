# Set project name
set(PROJECT_NAME    "MonsterEditor")
project(${PROJECT_NAME} VERSION 1.0.1)

include_directories(${QT_INCLUDE_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../commonEditor.UI/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../commonEditor.UI/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Models/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Storage/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../monsterEditor.Controllers/include)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

#Generate QT Form headers
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS AUTOGEN_BUILD_DIR/include)

set (RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../commonEditor.UI/res/darkstyle)

set(PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/aboutBoxForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/imageAttributionsForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/mainForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/manageTexturesForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editTextureForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/editMonsterForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/texturePickerForm.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/../commonEditor.UI/res/darkstyle/darkstyle.qrc
                         ${CMAKE_CURRENT_SOURCE_DIR}/res/global.qrc)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SUPPORT_MAYBE_UNUSED PROJECT_VERSION="${PROJECT_VERSION}")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CONAN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} MonsterEditor.Controllers theWarrior.Models theWarrior.Storage commonEditor.UI Qt5::Widgets boost_program_options)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") #gcc
  # https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
  target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wfatal-errors" # stops on first error
    "-Wall" # enables all the warnings about constructions
    "-Wextra" # Print extra warning messages"
    "-pedantic" # Issue all the warnings demanded by strict ISO C and ISO C++
    "-Wshadow" # Warn the user if a variable declaration shadows one from a parent context
    "-Wnon-virtual-dtor" # Warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    "-Wold-style-cast" # Warn for c-style casts
    "-Wcast-align" # Warn for potential performance problem casts
    "-Wunused" # warn on anything being unused
    "-Woverloaded-virtual" # warn if you overload (not override) a virtual function
    "-Wpedantic" # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
    "-Wconversion" # warn on type conversions that may lose data
    "-Wsign-conversion" # (Clang all versions, GCC >= 4.3) warn on sign conversions
    "-Wmisleading-indentation" # (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
    "-Wduplicated-cond" # (only in GCC >= 6.0) warn if if / else chain has duplicated conditions
    "-Wduplicated-branches" # (only in GCC >= 7.0) warn if if / else branches have duplicated code
    "-Wlogical-op" # (only in GCC) warn about logical operations being used where bitwise were probably wanted
    "-Wnull-dereference" # (only in GCC >= 6.0) warn if a null dereference is detected
    "-Wdouble-promotion" # (GCC >= 4.6, Clang >= 3.8) warn if float is implicit promoted to double
    "-Wformat=2" # warn on security issues around functions that format output (ie printf)
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wfatal-errors" # stops on first error
    "-Wall" # enables all the warnings about constructions
    "-Wextra" # Print extra warning messages"
    "-Werror" # Make all warnings into errors.
    "-Wno-unused-lambda-capture" # FIX It is necessary when building on MSVC
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

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES "res/MonsterEditor Icon16.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon32.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon48.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon64.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon128.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon256.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/MonsterEditor Icon512.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps" RENAME "thewarrior_monstereditor.png")
install(FILES "res/thewarrior.monstereditor.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/resources)
