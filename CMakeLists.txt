cmake_minimum_required(VERSION 3.13)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

include_directories(
        include
)

set(SOURCE_FILES
        src/factory.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/package.cpp
        src/storage_types.cpp
)

add_executable(NetSim_debug ${SOURCE_FILES} main.cpp)

set(SOURCES_FILES_TESTS
        test/test_Factory.cpp
)

add_executable(NetSim_test_factory ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_include_directories(NetSim_test_factory PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
)

add_subdirectory(googletest-master)

target_link_libraries(NetSim_test_factory gtest gmock)