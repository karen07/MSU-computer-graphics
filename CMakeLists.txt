cmake_minimum_required(VERSION 3.13)
project(computer-graphics)

add_compile_options(-Wall -Wextra -Werror -Wpedantic)
add_link_options()

file(GLOB SRC "src/*.cpp" "glslprogram/*.cpp" "mesh/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} GLEW glfw OpenGL lodepng)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)

find_program(CLANGFORMAT clang-format)
if(CLANGFORMAT)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/glslprogram/* ${CMAKE_CURRENT_SOURCE_DIR}/mesh/* ${CMAKE_CURRENT_SOURCE_DIR}/src/*
    )
endif()
