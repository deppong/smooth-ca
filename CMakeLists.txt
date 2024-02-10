cmake_minimum_required(VERSION 3.18...3.21)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(sdl_template LANGUAGES C CXX)

# GLOB RECURSE get all source files, remove or change as needed
file(
    GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# find and link sdl2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main)