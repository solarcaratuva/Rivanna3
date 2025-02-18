file(GLOB_RECURSE TOUCHGFX_SOURCES
    App/*.cpp
    generated/*.cpp
    gui/*.cpp
    target/*.cpp
)

add_library(touchgfx-lib STATIC ${TOUCHGFX_SOURCES})

target_include_directories(touchgfx-lib PUBLIC
    App
    config
    generated
    gui
    target
    target/generated
    generated/gui_generated/include
    gui/include/gui/common
    generated/fonts/include/fonts
)
