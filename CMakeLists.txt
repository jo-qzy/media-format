cmake_minimum_required(VERSION 3.13)

project(libflv)

option(BUILD_TEST "build example executable file" OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LIBFLV_NAME "libflv")
    add_compile_options(/wd4996)
else ()
    set(LIBFLV_NAME "flv")
endif ()

# Build libamf
add_subdirectory(deps/libamf)

include_directories(include)
include_directories(deps/libamf/include)

# Build LIBFLV
aux_source_directory(src SOURCE_FILES)

add_library(libflv STATIC ${SOURCE_FILES})
set_target_properties(libflv PROPERTIES OUTPUT_NAME ${LIBFLV_NAME})

add_library(libflv_shared SHARED ${SOURCE_FILES})
set_target_properties(libflv_shared PROPERTIES OUTPUT_NAME ${LIBFLV_NAME})

# Build test
if (BUILD_TEST)
    aux_source_directory(test TEST_SOURCE_FILES)

    foreach (TEST_FILE ${TEST_SOURCE_FILES})
        get_filename_component(TEST_BIN ${TEST_FILE} NAME_WE)

        add_executable(${TEST_BIN} ${TEST_FILE})
        target_link_libraries(${TEST_BIN} libflv)

        install(TARGETS ${TEST_BIN} DESTINATION ${CMAKE_INSTALL_PREFIX}/libflv/bin)
        add_test(NAME ${TEST_BIN} COMMAND ${TEST_BIN})
    endforeach ()
endif ()

# Install library and binary
install(TARGETS libflv libflv_shared
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/libflv/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/libflv/lib)

file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/libflv/include)
