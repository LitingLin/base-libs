cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

message(STATUS ${CMAKE_PREFIX_PATH})

option(BUILD_PLATFORM_SPECIFIC_FUNCTIONS OFF "Build OS specific functions")

set(SRC_FILES
        "${CMAKE_CURRENT_LIST_DIR}/include/base/common.h"
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

        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/encoder.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/common.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/types.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/jpeg.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/png.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/decoder/webp.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/encoder/webp.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/ext/img_codecs/encoder/jpeg.h"

        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/base.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/common.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/interface.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/macros.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/sinks/file.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/sinks/interface.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/sinks/stdout.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/dispatcher/dispatcher.h"

        "${CMAKE_CURRENT_LIST_DIR}/include/base/error_codes.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/base/error_codes/stringify.h"

        "${CMAKE_CURRENT_LIST_DIR}/src/base/exception.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/file.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/memory_mapped_io.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/decoder.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/encoder.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/decoder/jpeg.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/decoder/jpeg_turbo.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/decoder/png.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/decoder/webp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/encoder/webp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/ext/img_codecs/encoder/jpeg.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/stack_trace.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/utils.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/memory_alignment.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/include/base/cpu_info.h"

        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/base.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/interface.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/macros.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/sinks/file.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/sinks/stdout.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/dispatcher/dispatcher.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/src/base/error_codes/stringify.cpp"

        #"${CMAKE_CURRENT_LIST_DIR}/include/base/config/ini.h"
        #"${CMAKE_CURRENT_LIST_DIR}/include/base/config/ini_parser.h"

        #"${CMAKE_CURRENT_LIST_DIR}/src/base/config/ini.c"
        #"${CMAKE_CURRENT_LIST_DIR}/src/base/config/ini_parser.cpp"
        )

if(CMAKE_COMPILER_IS_GNUCXX)
    list(APPEND SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/src/base/cpu_info/gcc.cpp")
endif()
if(MSVC)
    list(APPEND SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/src/base/cpu_info/msvc.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/base/cpu_info/msvc.cpp")
endif()
if(WIN32)
    list(APPEND SRC_FILES
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/win32.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/win32/utils.h"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/logging/sinks/win32_debugger.h"

            "${CMAKE_CURRENT_LIST_DIR}/include/base/error_codes/win32/stringify.h"

            "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/sinks/win32_debugger.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/src/base/logging/win32/utils.cpp"

            "${CMAKE_CURRENT_LIST_DIR}/src/base/error_codes/win32/stringify.cpp"
            )

    if (${BUILD_SHARED_LIB})
        list(APPEND SRC_FILES
                "${CMAKE_CURRENT_LIST_DIR}/src/base/dll_entry.cpp")
    endif()
    if(${BUILD_PLATFORM_SPECIFIC_FUNCTIONS})
        list(APPEND SRC_FILES
                "${CMAKE_CURRENT_LIST_DIR}/include/base/com.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/d2d_window.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/dump_generator.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/crypto.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/event.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/fragmented_udp.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/image_decoder.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/message_box.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/message_notifier.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/mutex.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/native_event_looper.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/base/network.h"
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

                "${CMAKE_CURRENT_LIST_DIR}/src/base/com.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/d2d_window.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/dump_generator.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/crypto.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/event.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/fragmented_udp.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/image_decoder.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/message_box.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/mutex.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/native_event_looper.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/network.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/process.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/registry.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/security_attributes.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/shared_memory.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/sync.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/thread.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/time.cpp"
                "${CMAKE_CURRENT_LIST_DIR}/src/base/timer.cpp"
                )
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/fmt.cmake)

include(ExternalProject)
ExternalProject_Add(mozjpeg
        PREFIX ${CMAKE_BINARY_DIR}/mozjpeg
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DENABLE_SHARED=FALSE -DWITH_TURBOJPEG=FALSE -DPNG_SUPPORTED=FALSE -DWITH_JPEG8=1
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
        -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
        -DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}
        -DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
        -DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
        -DCMAKE_STATIC_LINKER_FLAGS=${CMAKE_STATIC_LINKER_FLAGS}
        -DCMAKE_STATIC_LINKER_FLAGS_DEBUG=${CMAKE_STATIC_LINKER_FLAGS_DEBUG}
        -DCMAKE_STATIC_LINKER_FLAGS_RELEASE=${CMAKE_STATIC_LINKER_FLAGS_RELEASE}
        -DCMAKE_STATIC_LINKER_FLAGS_MINSIZEREL=${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL}
        -DCMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO=${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}
        -DCMAKE_MODULE_LINKER_FLAGS=${CMAKE_MODULE_LINKER_FLAGS}
        -DCMAKE_MODULE_LINKER_FLAGS_DEBUG=${CMAKE_MODULE_LINKER_FLAGS_DEBUG}
        -DCMAKE_MODULE_LINKER_FLAGS_RELEASE=${CMAKE_MODULE_LINKER_FLAGS_RELEASE}
        -DCMAKE_MODULE_LINKER_FLAGS_MINSIZEREL=${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL}
        -DCMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO=${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_EXE_LINKER_FLAGS_DEBUG=${CMAKE_EXE_LINKER_FLAGS_DEBUG}
        -DCMAKE_EXE_LINKER_FLAGS_RELEASE=${CMAKE_EXE_LINKER_FLAGS_RELEASE}
        -DCMAKE_EXE_LINKER_FLAGS_MINSIZEREL=${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}
        -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO=${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS_DEBUG=${CMAKE_SHARED_LINKER_FLAGS_DEBUG}
        -DCMAKE_SHARED_LINKER_FLAGS_RELEASE=${CMAKE_SHARED_LINKER_FLAGS_RELEASE}
        -DCMAKE_SHARED_LINKER_FLAGS_MINSIZEREL=${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL}
        -DCMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO=${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd_party/mozjpeg
        )

list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_JPEG)
list(APPEND BASE_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)
list(APPEND BASE_LINK_DIRECTORIES ${CMAKE_BINARY_DIR}/lib)
if (MSVC)
    list(APPEND BASE_LINK_LIBRARIES jpeg-static)
else()
    list(APPEND BASE_LINK_LIBRARIES libjpeg.a)
endif()

find_package(PNG)
if(PNG_FOUND)
    list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_PNG)
    list(APPEND BASE_INCLUDE_DIRS ${PNG_INCLUDE_DIRS})
    list(APPEND BASE_LINK_LIBRARIES ${PNG_LIBRARIES})
else()
    message(STATUS "Disabled PNG decoder")
endif()

find_package(WebP)
if(WEBP_FOUND)
    list(APPEND BASE_COMPILE_DEFINITIONS HAVE_LIB_WEBP)
    list(APPEND BASE_INCLUDE_DIRS ${WEBP_INCLUDE_DIRS})
    list(APPEND BASE_LINK_LIBRARIES ${WEBP_LIBRARIES})
else()
    message(STATUS "Disabled WEBP codecs")
endif()

list(APPEND BASE_LINK_LIBRARIES fmt)
list(APPEND BASE_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include ${FMT_INCLUDE_DIRS})
if (WIN32)
    list(APPEND BASE_COMPILE_DEFINITIONS UNICODE _UNICODE)
endif()

option(BUILD_SHARED_LIB "Building shared library" ON)

if (BUILD_SHARED_LIB)
    add_library(base SHARED ${SRC_FILES})
    target_compile_definitions(base PRIVATE COMPILING_DLL COMPILING_IMAGE_CODECS COMPILING_LOGGING)
    if (WIN32)
        target_compile_definitions(base PRIVATE _WINDLL)
    endif()
    set_target_properties(base PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(base PROPERTIES C_VISIBILITY_PRESET hidden)
else()
    add_library(base STATIC ${SRC_FILES})
endif()

target_compile_definitions(base PUBLIC ${BASE_COMPILE_DEFINITIONS})
target_include_directories(base PUBLIC ${BASE_INCLUDE_DIRS})
target_link_directories(base PUBLIC ${BASE_LINK_DIRECTORIES})
target_link_libraries(base ${BASE_LINK_LIBRARIES} fmt)
add_dependencies(base fmt mozjpeg)

option(RELEASE_INSTALL "install binary files only" OFF)

if (${RELEASE_INSTALL})
    if (${BUILD_SHARED_LIB})
        install(TARGETS base
                RUNTIME DESTINATION .
                LIBRARY DESTINATION .)
    endif()
else()
    install(TARGETS base fmt
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)

    install(DIRECTORY include/ DESTINATION include)
endif()

option(BUILD_TEST "Build the tests" OFF)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

set_property(GLOBAL PROPERTY BASE_COMPILE_DEFINITIONS ${BASE_COMPILE_DEFINITIONS})
set_property(GLOBAL PROPERTY BASE_INCLUDE_DIRS ${BASE_INCLUDE_DIRS})
set_property(GLOBAL PROPERTY BASE_LINK_DIRECTORIES ${BASE_LINK_DIRECTORIES})
set_property(GLOBAL PROPERTY BASE_LINK_LIBRARIES ${BASE_LINK_LIBRARIES})
