project(media_core)

aux_source_directory(. SOURCES)

add_library(${PROJECT_NAME} OBJECT ${SOURCES})
list(APPEND MFA_OBJS $<TARGET_OBJECTS:${PROJECT_NAME}>)
set(MFA_OBJS ${MFA_OBJS} PARENT_SCOPE)