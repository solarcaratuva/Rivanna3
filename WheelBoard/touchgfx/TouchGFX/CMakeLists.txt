file(GLOB_RECURSE TOUCHGFX_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(touchgfx_core STATIC ${TOUCHGFX_CORE_SOURCES})

target_include_directories(touchgfx_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
