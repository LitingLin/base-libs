cmake_minimum_required(VERSION 3.1)

find_package(GTest)

if(GTEST_FOUND)
    if (WIN32)
        set(TEST_SRC_FILES test.cpp pch.cpp test_random.cpp)
    else()
        set(TEST_SRC_FILES pch.cpp test_random.cpp)
    endif()
    add_executable(base-lib-test ${TEST_SRC_FILES})
    target_compile_definitions(base-lib-test PRIVATE ${BASE_COMPILE_DEFINITIONS})
    target_include_directories(base-lib-test PRIVATE ${BASE_INCLUDE_DIRS})
    target_link_libraries(base-lib-test ${BASE_LINK_LIBRARIES})
    target_include_directories(base-lib-test PRIVATE ${GTEST_INCLUDE_DIR})
    target_link_libraries(base-lib-test ${GTEST_BOTH_LIBRARIES} base pthread)

    add_test(
            NAME base-lib-test
            COMMAND base-lib-test
    )
else()
    message(STATUS "GTest not found. Unit test module disabled.")
endif()