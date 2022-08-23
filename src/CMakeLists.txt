cmake_minimum_required(VERSION 3.18)

project(sdl_template)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(sdl_template main.c)
if(MSVC)
    target_link_libraries(sdl_template ${SDL2_LIBRARIES})
    target_compile_options(sdl_template PRIVATE /W4)
else()
    target_link_libraries(sdl_template ${SDL2_LIBRARIES} -ldl -lm)
    target_compile_options(sdl_template PRIVATE -Wall -Wextra)
endif()