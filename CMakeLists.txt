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
            "${CMAKE_CURRENT_LIST_DIR}/include/base/crypto.h"
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
            "${CMAKE_CURRENT_LIST_DIR}/include/base/data_structures.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/random.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/memory_alignment.h"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/jpeg.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/png.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/webp.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/encoder/webp.h"

            "${CMAKE_CURRENT_LIST_DIR}/src/config/ini.c"
            "${CMAKE_CURRENT_LIST_DIR}/src/config/ini_parser.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging/win32.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/com.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/d2d_window.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/debugoutput_logger_sink.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/dump_generator.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/crypto.cpp"
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
            "${CMAKE_CURRENT_LIST_DIR}/src/stack_trace.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/sync.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/thread.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/time.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/timer.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_alignment.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/jpeg.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/png.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/webp_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/encoder/webp_encoder.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info/msvc.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info/msvc.cpp")
else()
    set(SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/include/base/data_structures.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/random.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/memory_alignment.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/exception.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/file.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/image_decoder.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/memory_mapped_io.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/porting.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/preprocessor.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/utils.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/jpeg.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/png.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/webp.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/encoder/webp.h"

            "${CMAKE_CURRENT_LIST_DIR}/src/exception.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/file.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/logging.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_mapped_io.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/jpeg.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/png.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/decoder/webp_decoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/ext/img_codecs/encoder/webp_encoder.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/stack_trace.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/memory_alignment.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info/gcc.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/cpu_info/gcc.cpp")
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(cereal)
if(NOT CEREAL_FOUND)
    message(STATUS "Using internal provided cereal library")
    include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/cereal.cmake)
endif()

find_package(spdlog)
if(NOT SPDLOG_FOUND)
    message(STATUS "Using internal provided spdlog library")
    include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/spdlog.cmake)
endif()


find_package(TurboJPEG)
if(TURBOJPEG_FOUND)
    list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_JPEG_TURBO)
    list(APPEND BASE_INCLUDE_DIRS ${TURBOJPEG_INCLUDE_DIR})
    list(APPEND BASE_LINK_LIBRARIES ${TURBOJPEG_LIBRARY})
else()
    message(STATUS "Disabled TurboJPEG decoder")
endif()

find_package(PNG)
if(PNG_FOUND)
    list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_PNG)
    list(APPEND BASE_INCLUDE_DIRS ${PNG_INCLUDE_DIRS})
    list(APPEND BASE_LINK_LIBRARIES ${PNG_LIBRARIES})
else()
    message(STATUS "Disabled TurboJPEG decoder")
endif()

find_package(WEBP)
if(WEBP_FOUND)
    list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_WEBP)
    list(APPEND BASE_INCLUDE_DIRS ${WEBP_INCLUDE_DIRS})
    list(APPEND BASE_LINK_LIBRARIES ${WEBP_LIBRARIES})
else()
    message(STATUS "Disabled WEBP codecs")
endif()

list(APPEND BASE_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include ${CEREAL_INCLUDE_DIRS} ${SPDLOG_INCLUDE_DIRS})
if (WIN32)
    list(APPEND BASE_COMPILE_DEFINITIONS UNICODE _UNICODE)
    list(APPEND BASE_LINK_LIBRARIES d2d1 strmiids)
endif()

option(BUILD_SHARED_LIBS "Build as shared library" OFF)
if(BUILD_SHARED_LIBS)
    set(BUILDING_TYPE SHARED)
else()
    set(BUILDING_TYPE STATIC)
endif()

add_library(base ${BUILDING_TYPE} ${SRC_FILES})

target_compile_definitions(base PUBLIC ${BASE_COMPILE_DEFINITIONS})
target_include_directories(base PUBLIC ${BASE_INCLUDE_DIRS})
target_link_libraries(base ${BASE_LINK_LIBRARIES})

install(TARGETS base
        ARCHIVE DESTINATION "lib")

install(DIRECTORY include/ DESTINATION include)

if(NOT CEREAL_FOUND)
    install(DIRECTORY ${CEREAL_INCLUDE_DIRS} DESTINATION include)
endif()

if(NOT SPDLOG_FOUND)
    install(DIRECTORY ${SPDLOG_INCLUDE_DIRS} DESTINATION include)
endif()

option(BUILD_TEST "Build the tests" ON)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
