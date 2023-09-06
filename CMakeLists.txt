cmake_minimum_required(VERSION 3.26)

# general informations
project("clikor"
        VERSION 0.0.1.20230819
        DESCRIPTION "clicks for you"
        HOMEPAGE_URL https://github.com/tetsuhaut/clikor
        LANGUAGES CXX)

# the project language is C++ 23
set(CMAKE_CXX_STANDARD 23)

# if the compiler doesn't know C++ 23, fail
set(CMAKE_CXX_STANDARD_REQUIRED 23)

# no language extension to be as portable as possible
set(CMAKE_CXX_EXTENSIONS OFF)

# experimental use of C++ 20 modules
# need to update at each new CMake version
# see https://github.com/Kitware/CMake/blob/v<current cmake version>/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")

# uncomment in case we want to know where CMake is looking for packages
#set(CMAKE_FIND_DEBUG_MODE 1)

# building in-tree is not allowed
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree built. Remove CMakeCache.txt, the CMakeFiles directory and create a build directory outside of the source code and call cmake from there.")
endif()

# set build type, "Debug" by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "build type (one of Debug, RelWithDebInfo, Release, MinSizeRel)" FORCE)
endif()

# use the UTF-8 encoding internally
option(CMAKE_ENCODING_UTF8 "Use UTF-8 encoding internally." ON)

# where to produce executables and libraries
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# create a variable containing the source file without the main entry point
# we'll use it (one day) to build a test program
file(GLOB_RECURSE mainSourceFiles src/main/cpp/*)
list(REMOVE_ITEM mainSourceFiles src/main/cpp/main.cpp)

# the source code creates an executable file named after the project name
add_executable(${PROJECT_NAME})

# the executable file is created from main.cpp and modules
target_sources(${PROJECT_NAME} PUBLIC src/main/cpp/main.cpp)
target_sources(${PROJECT_NAME}
  PUBLIC
  FILE_SET all_my_modules TYPE CXX_MODULES FILES
  ${mainSourceFiles}
)

# pass build informations to the source code
target_compile_definitions(${PROJECT_NAME} PUBLIC APP_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(${PROJECT_NAME} PUBLIC APP_NAME="${CMAKE_PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME} PUBLIC APP_VENDOR="tetsuhaut")
target_compile_definitions(${PROJECT_NAME} PUBLIC MAIN_EXECUTABLE_NAME="${PROJECT_NAME}")

################################################################################
# library configurations
################################################################################

################################################################################
# will use the FLTK libraries, so configure the project for it
# need to have FLTK_DIR as an environment variable pointing to the dir where
# the library has been installed
# notes:
# - CMake's findFltk tool does not point on the installed version, so can't use it
# - FLTK cmake conf does not provide version :(
################################################################################
set(FLTK_SKIP_FORMS true)
set(FLTK_SKIP_FLUID true)
set(FLTK_SKIP_IMAGES true)
set(FLTK_SKIP_OPENGL true)
find_package(FLTK REQUIRED) # dunno how to provide version
target_include_directories(${PROJECT_NAME} PRIVATE ${FLTK_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FLTK_LIBRARIES})

################################################################################
# will use the GSL library https://github.com/Microsoft/GSL, so configure the
# project for it.
# need to have Microsoft.GSL_DIR as an environment variable pointing to the dir
# where the library has been installed
################################################################################
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft.GSL::GSL)

include(FetchContent)
################################################################################
# will use the input_lite library https://github.com/smasherprog/input_lite, so
# configure the project for it.
# need to have input_lite_DIR as an environment variable pointing to the dir
# where the library has been installed
################################################################################
FetchContent_Declare(
  input_lite
  GIT_REPOSITORY https://github.com/smasherprog/input_lite.git
  GIT_TAG 3.0.0
)
FetchContent_GetProperties(input_lite)
if(NOT input_lite_POPULATED)
  FetchContent_Populate(input_lite)
  set(BUILD_EXAMPLE OFF CACHE INTERNAL "disable building examples")
  set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "link statically")
  add_subdirectory(${input_lite_SOURCE_DIR} ${input_lite_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{input_lite_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE ${INPUT_LITE_LIBRARY})

################################################################################
# compiler configurations
################################################################################

################################################################################
# if used with Microsoft Visual Studio, link statically, compile with max
# warnings, compile with multiple threads (i.e. faster), use utf-8 without byte
# order mark, use the incremental build.
# see https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
################################################################################
if(MSVC)
  # link statically
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # select the ${PROJECT_NAME} project when opening Visual Studio
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  
  # use utf-8 without byte order mark, compile with multiple threads
  # note : /INCREMENTAL is is implied for debug build
  # /fsanitize=address, /fsanitize=undefined, /fsanitize=memory, /fsanitize=thread, /fsanitize=leak, /fsanitize=hwaddress
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /MP /ZI /Gy /EHsc /Zc:__cplusplus /fsanitize=leak" CACHE STRING "" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099" CACHE STRING "" FORCE)
  # compile in pure c++, with max possible warning level
  target_compile_options(${PROJECT_NAME} PRIVATE
                                                 /JMC # debug just mt code
                                                 /utf-8 # Set source and execution character sets to UTF-8.
                                                 /MP # Builds multiple source files concurrently.
                                                 /Zc:__cplusplus # the __cplusplus macro tells the C++ version is use
                                                 /ZI # Edit and Continue debugging features
                                                 /permissive- # standards-conforming compiler
                                                 /std:c++latest # use the latest C++ version
                                                 /Wall # compile with max warning level
                                                 /wd4623 # do not warn about suppressed default constructors
                                                 /wd4625 # do not warn about suppressed copy constructor
                                                 /wd4626 # do not warn about suppressed assignment operator
                                                 /wd4820 # do not warn about memory padding
                                                 /wd5026 # do not warn about suppressed default move constructor
                                                 /wd5027 # do not warn about suppressed move assignment constructor
                                                 /wd5045 # do not warn about inserting Spectre mitigation
                                                 /wd5246 # do not warn about using braces ti initialize std::array
  )

################################################################################
# if used with MinGW, link statically, compile with max warnings, do not show
# the console window for reviewer.exe, use the Windows API, the thread API
# Note: if using https://gcc-mcf.lhmouse.com/, the executable must be deployed
# aside of the mcfgthread DLL, because it can't be statically linked. See 
# https://sourceforge.net/p/mingw-w64/mailman/mingw-w64-public/thread/575d13dd.430ac20a.33013.1aa0@mx.google.com/
################################################################################
elseif(MINGW)
  # link statically
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  target_link_libraries(${PROJECT_NAME} PRIVATE "-lpsapi -pthread")
  add_definitions(-DWIN32)
  # compile in pure c++, with max possible warning level
  target_compile_options(${PROJECT_NAME} PRIVATE
                                                 -std=c++2a
                                                 -Wall
                                                 -Wextra
                                                 -Wpedantic
                                                 -Werror=unused-function
                                                 -pedantic-errors
                                                 -Wvla
                                                 -Wextra-semi
                                                 -Wnull-dereference
                                                 -Wswitch-enum
                                                 -Wno-deprecated
                                                 -fno-elide-type
                                                 -Wduplicated-cond
                                                 -Wduplicated-branches
                                                 -Wsuggest-override
                                                 -Wshadow
                                                 -Wnon-virtual-dtor
                                                 -Wold-style-cast
                                                 -Wcast-align
                                                 -Wunused
                                                 -Woverloaded-virtual
                                                 -pedantic -Wconversion
                                                 -Wsign-conversion
                                                 -Wmisleading-indentation
                                                 -Wduplicated-cond
                                                 -Wduplicated-branches
                                                 -Wlogical-op
                                                 -Wnull-dereference
                                                 -Wuseless-cast
                                                 -Wdouble-promotion
                                                 -Wformat=2
                                                 -Weffc++
                                                 -Werror=conversion
  )
endif(MSVC)