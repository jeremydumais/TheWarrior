# Set project name
set(PROJECT_NAME    "MapEditor.Controllers")

include_directories(${QT_INCLUDE_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../twModels/include)

                    set(PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/mainController.cpp
                                             ${CMAKE_CURRENT_SOURCE_DIR}/src/glComponentController.cpp
                                             ${CMAKE_CURRENT_SOURCE_DIR}/src/textureSelectionComponentController.cpp)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_FILES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
target_compile_definitions(${PROJECT_NAME} PRIVATE SUPPORT_MAYBE_UNUSED)
endif()

#target_link_libraries(${PROJECT_NAME} Models DataServices Controllers Qt5::Widgets sqlite3)

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

if (NOT DEFINED BUILD_TESTING OR BUILD_TESTING)
add_subdirectory("test")
endif()


install(TARGETS ${PROJECT_NAME} DESTINATION lib)