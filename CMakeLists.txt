cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(SRC_FILES
            "src/com.cpp"
            "src/d2d_window.cpp"
            "src/debugoutput_logger_sink.cpp"
            "src/dump_generator.cpp"
            "src/encryption.cpp"
            "src/event.cpp"
            "src/exception.cpp"
            "src/file.cpp"
            "src/fragmented_udp.cpp"
            "src/image_decoder.cpp"
            "src/lock_guard.cpp"
            "src/logging.cpp"
            "src/memory_mapped_io.cpp"
            "src/message_box.cpp"
            "src/mutex.cpp"
            "src/native_event_looper.cpp"
            "src/network.cpp"
            "src/process.cpp"
            "src/registry.cpp"
            "src/security_attributes.cpp"
            "src/shared_memory.cpp"
            "src/stack_track.cpp"
            "src/sync.cpp"
            "src/thread.cpp"
            "src/time.cpp"
            "src/timer.cpp"
            "src/utils.cpp")
else()
    set(SRC_FILES
            "src/exception.cpp"
            "src/file.cpp"
            "src/logging.cpp"
            "src/memory_mapped_io.cpp"
            "src/stack_trace.cpp"
            "src/utils.cpp")
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
