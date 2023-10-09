# Set project name
set(PROJECT_NAME    "MonsterEditor.Controllers")

include_directories(${QT_INCLUDE_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../commonEditor.UI/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Storage/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../theWarrior.Models/include)

                    set(PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/mainController.cpp
                                             ${CMAKE_CURRENT_SOURCE_DIR}/src/texturePickerController.cpp
                                             ${CMAKE_CURRENT_SOURCE_DIR}/src/manageMonsterController.cpp)

find_package(Qt5 COMPONENTS Gui OpenGL REQUIRED)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_FILES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SUPPORT_MAYBE_UNUSED)
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CONAN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} theWarrior.Models theWarrior.Storage commonEditor.UI Qt5::Widgets)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") #gcc
# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
target_compile_options(${PROJECT_NAME}
PRIVATE "-Wfatal-errors" # stops on first error
"-Wall" # enables all the warnings about constructions
"-Wextra" # Print extra warning messages)"
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
"-Wuseless-cast" # (only in GCC >= 4.8) warn if you perform a cast to the same type
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

if (NOT DEFINED BUILD_TESTING OR BUILD_TESTING)
    add_subdirectory("test")
endif()


install(TARGETS ${PROJECT_NAME} DESTINATION lib)
