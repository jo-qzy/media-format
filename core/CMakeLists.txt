project(media_core)

set(SOURCES
    media_io.cpp
    mfa_buf.cpp
    mfa_data.cpp)

if (ENABLE_LOG)
    list(APPEND SOURCES mfa_log.cpp)
endif ()

add_library(${PROJECT_NAME} OBJECT ${SOURCES})
list(APPEND MFA_OBJS $<TARGET_OBJECTS:${PROJECT_NAME}>)
set(MFA_OBJS ${MFA_OBJS} PARENT_SCOPE)