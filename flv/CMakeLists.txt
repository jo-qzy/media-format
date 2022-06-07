cmake_minimum_required(VERSION 3.12)

project(flv)

# Build flv
aux_source_directory(. SOURCES)
add_library(${PROJECT_NAME} OBJECT ${SOURCES})
list(APPEND MFA_OBJS $<TARGET_OBJECTS:${PROJECT_NAME}>)
set(MFA_OBJS ${MFA_OBJS} PARENT_SCOPE)

# Build test
if (BUILD_TEST)
    aux_source_directory(test TEST_SOURCES)
    add_library(${PROJECT_NAME}_test OBJECT ${TEST_SOURCES})
    list(APPEND MFA_TEST_OBJS $<TARGET_OBJECTS:${PROJECT_NAME}_test>)
    set(MFA_TEST_OBJS ${MFA_TEST_OBJS} PARENT_SCOPE)
endif ()
