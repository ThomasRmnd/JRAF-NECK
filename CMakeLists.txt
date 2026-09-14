cmake_minimum_required(VERSION 3.16)

project(JRAF-NECK
    DESCRIPTION "JUNO Reactor Analysis Framework - NTuples Event Cut Kit (JRAF-NECK)"
    LANGUAGES CXX
)

# --- Language standard -----------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for clangd / tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if nothing specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- Dependencies ------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

# --- Sources -----------------------------------------------------------------
# Prefer an explicit list over GLOB_RECURSE: CMake won't reconfigure
# automatically when you add/remove a .cpp file with glob, which bites you
# silently. If you want the convenience of glob during early development,
# keep it, but re-run `cmake ..` manually after adding files.
file(GLOB_RECURSE JRAF_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(jrafneck ${JRAF_SOURCES})

target_include_directories(jrafneck PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Use ROOT's imported target instead of manually injecting include dirs +
# raw library list — this pulls in include dirs, definitions and libs
# transitively and correctly, no dl needed on modern ROOT builds.
target_link_libraries(jrafneck PRIVATE ROOT::RIO ROOT::Net)

# --- Compiler warnings ---------------------------------------------------
# Scoped to the target instead of globally via CMAKE_CXX_FLAGS.
# Dropped -ansi -pedantic-errors: ROOT headers routinely violate strict
# ISO C++ and pedantic-errors will make your build fail inside ROOT's
# own headers, not just your code.
target_compile_options(jrafneck PRIVATE
    -Wall -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# --- Output location -------------------------------------------------------
set_target_properties(jrafneck PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)