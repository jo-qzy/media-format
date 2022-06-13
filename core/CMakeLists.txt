project(media_core)

set(SOURCES
    mfa_buf.c
    mfa_data.c)

if (ENABLE_LOG)
    list(APPEND SOURCES mfa_log.c)
endif ()

add_library(${PROJECT_NAME} OBJECT ${SOURCES})
list(APPEND MFA_OBJS $<TARGET_OBJECTS:${PROJECT_NAME}>)
set(MFA_OBJS ${MFA_OBJS} PARENT_SCOPE)