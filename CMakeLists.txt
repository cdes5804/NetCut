cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")   # For clang-tidy.
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 17)                                # Compile as C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)                       # Require C++17 support

project(NetCut
        VERSION 2023.6
        DESCRIPTION "NetCut"
        LANGUAGES C CXX
        )

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake ..\" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

# Build support scripts
set(BUILD_SUPPORT_DIR ${PROJECT_SOURCE_DIR}/build_support)
set(CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin")

# CTest
enable_testing()

# clang-format
if (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_FORMAT_BIN
            NAMES clang-format
            HINTS ${CLANG_SEARCH_PATH})
endif()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-format.")
else()
    message(STATUS "Found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# Compilation flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")
# Includes
set(SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

include_directories(${SRC_INCLUDE_DIR})
add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cc)
target_link_libraries(${PROJECT_NAME} netcut_shared bsd)

##########################################
# "make format"
# "make check-format"
##########################################

string(CONCAT NETCUT_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        )

# runs clang format and updates files in place.
add_custom_target(format ${BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${NETCUT_FORMAT_DIRS}
        --fix
        --quiet
        )

###########################################################
# "make check-coding-style" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories

add_custom_target(check-coding-style
        ${BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -p ${CMAKE_BINARY_DIR}                                     # using cmake's generated compile commands
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
add_dependencies(check-coding-style netcut_shared)                 # needs gtest headers, compile_commands.json