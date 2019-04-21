cmake_minimum_required(VERSION 3.1)

find_package(GTest)

if(GTEST_FOUND)
    enable_testing()
    add_executable(base-lib-test ${SRC_FILES} test.cpp pch.cpp)
    target_include_directories(base-lib-test PRIVATE ${GTEST_INCLUDE_DIR})
    target_include_directories(base-lib-test PRIVATE ../include)
    target_link_libraries(base-lib-test ${GTEST_BOTH_LIBRARIES})

    add_test(
            NAME base-lib-test
            COMMAND base-lib-test
    )
else()
    message(STATUS "GTest not found. Unit test module disabled.")
endif()