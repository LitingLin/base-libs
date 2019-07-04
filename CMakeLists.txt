cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
message(STATUS ${CMAKE_PREFIX_PATH})
if(WIN32)
    set(SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/include/base/com.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/d2d_window.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/debugoutput_logger_sink.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/dump_generator.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/encryption.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/event.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/exception.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/file.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/fragmented_udp.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/image_decoder.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/lock_guard.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/memory_mapped_io.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/message_box.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/message_notifier.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/mutex.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/native_event_looper.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/network.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/porting.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/preprocessor.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/process.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/registry.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/rw_spin_lock.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/security_attributes.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/serialization.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/shared_memory.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/sync.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/thread.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/time.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/timer.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/types.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/utils.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/win32.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/config/ini.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/config/ini_parser.h"

            "${CMAKE_CURRENT_LIST_DIR}/src/config/ini.c"
            "${CMAKE_CURRENT_LIST_DIR}/src/config/ini_parser.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging/win32.cpp"
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
            "${CMAKE_CURRENT_LIST_DIR}/src/lib_jpeg_turbo_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/lib_png_decoder.cpp"
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
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info/msvc.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info/msvc.cpp")
else()
    set(SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/include/base/exception.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/file.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/image_decoder.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/memory_mapped_io.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/porting.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/preprocessor.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/utils.h"

            "${CMAKE_CURRENT_LIST_DIR}/src/exception.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/file.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_mapped_io.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/lib_jpeg_turbo_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/lib_png_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/stack_trace.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info/gcc.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info/gcc.cpp"
            )
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

find_package(TurboJPEG)
if(TURBOJPEG_FOUND)
    target_compile_definitions(base PUBLIC HAVE_LIB_JPEG_TURBO)
    target_include_directories(base PUBLIC ${TURBOJPEG_INCLUDE_DIR})
    target_link_libraries(base ${TURBOJPEG_LIBRARY})
else()
    message(STATUS "Disabled TurboJPEG decoder")
endif()

find_package(PNG)
if(PNG_FOUND)
    target_compile_definitions(base PUBLIC HAVE_LIB_PNG)
    target_include_directories(base PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(base ${PNG_LIBRARIES})
else()
    message(STATUS "Disabled TurboJPEG decoder")
endif()

target_include_directories(base PUBLIC "include")
target_include_directories(base PUBLIC ${CEREAL_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})

install(TARGETS base
        ARCHIVE DESTINATION "lib")

install(DIRECTORY include/ DESTINATION include)

if(NOT CEREAL_FOUND)
    install(DIRECTORY ${CEREAL_INCLUDE_DIRS} DESTINATION .)
endif()

if(NOT SPDLOG_FOUND)
    install(DIRECTORY ${SPDLOG_INCLUDE_DIRS} DESTINATION .)
endif()

option(BUILD_TEST "Build the tests" OFF)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
