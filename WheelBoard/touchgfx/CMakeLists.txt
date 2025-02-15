file(GLOB_RECURSE TOUCHGFX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/App/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/target/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/target/*.cpp
)

add_library(touchgfx_lib STATIC ${TOUCHGFX_SOURCES})

# Specify the public include directories for TouchGFX.
target_include_directories(touchgfx_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/App
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/target
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)