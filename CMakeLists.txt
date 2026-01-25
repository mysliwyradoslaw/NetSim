cmake_minimum_required(VERSION 3.13)
project(implementation)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

find_package(GTest REQUIRED)

include_directories(
        include
        ${GTEST_INCLUDE_DIRS}
)

set(SOURCE_FILES
        src/factory.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/package.cpp
        src/storage_types.cpp
        )

add_executable(netsim_products ${SOURCE_FILES} main.cpp)

target_compile_definitions(netsim_products PUBLIC EXERCISE_ID=EXERCISE_ID_PACKAGES)

target_link_libraries(netsim_products GTest::gtest GTest::gtest_main)