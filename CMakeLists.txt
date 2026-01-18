cmake_minimum_required(VERSION 3.13)
project(implementation)

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

add_executable(netsim_products ${SOURCE_FILES} main.cpp)

add_executable(my_project_debug ${SOURCE_FILES} main.cpp)

set(SOURCES_FILES_TESTS
        test/test_Factory.cpp

        test/test_nodes.cpp
        test/test_package.cpp

        test/test_storage_types.cpp
)

add_executable(my_project_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS}
        test/main_gtest.cpp)

target_include_directories(my_project_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
)

add_subdirectory(googletest-master)

target_link_libraries(my_project_test gmock)

target_compile_definitions(netsim_products PUBLIC EXERCISE_ID=EXERCISE_ID_PACKAGES)