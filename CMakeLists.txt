cmake_minimum_required(VERSION 3.31)
project(SwordGL)

set(CMAKE_CXX_STANDARD 23)

add_executable(SwordGL
        src/Application.cpp
        dependencies/glad/src/glad.c
)

# root directory macro
target_compile_definitions(SwordGL PRIVATE
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

target_include_directories(SwordGL PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
)

target_link_directories(SwordGL PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-universal
)

target_link_libraries(SwordGL PRIVATE
        glfw3
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
)
