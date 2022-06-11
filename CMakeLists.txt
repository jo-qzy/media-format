cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project(mfa)

option(ENABLE_FLV "enable flv" ON)
option(BUILD_STATIC "build static library" ON)
option(BUILD_SHARED "build shared library" OFF)
option(BUILD_DEMO "build example executable file" ON)
option(BUILD_TEST "build test executable file" OFF)

set(MFA_OBJS)
set(MFA_HEADER_DIRS)
set(MFA_TEST_OBJS)
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME})

include_directories(.)

# build core
add_subdirectory(core)

if (ENABLE_FLV)
    add_subdirectory(flv)
    list(APPEND MFA_HEADER_DIRS flv)
endif ()

# build libmedia
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(MFA_LIBRARY lib${PROJECT_NAME})
    add_compile_options(/wd4996)
else ()
    set(MFA_LIBRARY ${PROJECT_NAME})
endif ()

if (BUILD_SHARED)
    add_library(${MFA_LIBRARY} SHARED ${MFA_OBJS})
else()
    add_library(${MFA_LIBRARY} STATIC ${MFA_OBJS})
endif()

if (BUILD_TEST)
    set(MFA_TEST ${PROJECT_NAME}_test)
    add_executable(${MFA_TEST} test/test.cpp ${MFA_TEST_OBJS})
    target_link_libraries(${MFA_TEST} ${MFA_LIBRARY})
    install(TARGETS ${MFA_TEST}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif ()

# Install library and binary
install(TARGETS ${MFA_LIBRARY})
install(DIRECTORY ${MFA_HEADER_DIRS}
    TYPE INCLUDE
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "test" EXCLUDE
    )
