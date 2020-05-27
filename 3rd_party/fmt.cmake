set(FMT_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/fmt/include/)
set(FMT_HEADERS
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/chrono.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/color.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/compile.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/core.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/format.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/format-inl.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/locale.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/os.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/ostream.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/posix.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/printf.h
        ${CMAKE_CURRENT_LIST_DIR}/fmt/include/fmt/ranges.h
        )
set(FMT_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/fmt/src/format.cc
        ${CMAKE_CURRENT_LIST_DIR}/fmt/src/os.cc)

add_library(fmt STATIC ${FMT_HEADERS} ${FMT_SOURCES})
target_include_directories(fmt PUBLIC ${FMT_INCLUDE_DIRS})
