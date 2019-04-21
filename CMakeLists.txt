cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/src/com.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/d2d_window.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/debugoutput_logger_sink.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/dump_generator.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/encryption.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/event.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/exception.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/file.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/fragmented_udp.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/image_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/lock_guard.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_mapped_io.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/message_box.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/mutex.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/native_event_looper.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/network.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/process.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/registry.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/security_attributes.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/shared_memory.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/stack_track.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/sync.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/thread.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/time.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/timer.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp")
else()
    set(SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/src/exception.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/file.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_mapped_io.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/stack_trace.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/config/ini.c")
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(cereal)
if(NOT CEREAL_FOUND)
    message(STATUS "Using internal provided cereal library")
    include(3rd_party/cereal.cmake)
endif()

find_package(spdlog)
if(NOT SPDLOG_FOUND)
    message(STATUS "Using internal provided spdlog library")
    include(3rd_party/spdlog.cmake)
endif()

add_library(base STATIC ${SRC_FILES})
target_include_directories(base PUBLIC "include")
target_include_directories(base PRIVATE ${CEREAL_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})

install(TARGETS base
        ARCHIVE DESTINATION "lib"
        PUBLIC_HEADER DESTINATION "include")

if(NOT CEREAL_FOUND)
    install(DIRECTORY ${CEREAL_INCLUDE_DIRS} DESTINATION "include")
endif()

if(NOT SPDLOG_FOUND)
    install(DIRECTORY ${SPDLOG_INCLUDE_DIRS} DESTINATION "include")
endif()

add_subdirectory(test)