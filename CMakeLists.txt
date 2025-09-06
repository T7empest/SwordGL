cmake_minimum_required(VERSION 3.31)
project(SwordGL)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_program(GLSLC glslc REQUIRED)
set(SHADER_SRC ${CMAKE_SOURCE_DIR}/res/shaders)
set(SHADER_BIN ${CMAKE_BINARY_DIR}/res/shaders)
file(MAKE_DIRECTORY ${SHADER_BIN})

function(glsl_to_spv NAME STAGE) # STAGE=vert|frag|comp
    set(IN_GLSL ${SHADER_SRC}/${NAME}.${STAGE})
    set(OUT_SPV ${SHADER_BIN}/${STAGE}.spv)
    add_custom_command(
            OUTPUT ${OUT_SPV}
            COMMAND ${GLSLC} -fshader-stage=${STAGE} -O -o ${OUT_SPV} ${IN_GLSL}
            DEPENDS ${IN_GLSL}
            COMMENT "GLSL -> SPIR-V: ${NAME}.${STAGE}")
    set(${NAME}_${STAGE}_SPV ${OUT_SPV} PARENT_SCOPE)
endfunction()

glsl_to_spv(shader vert)
glsl_to_spv(shader frag)

add_custom_target(shaders ALL
        DEPENDS ${shader_vert_SPV} ${shader_frag_SPV})

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/dependencies/imgui)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

set(IMGUI_BACKENDS
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdlgpu3.cpp
        src/graphics/UI.cc
        src/graphics/UI.h
        src/graphics/render_passes/imgui_pass.h
        src/graphics/particle_system.cc
        src/graphics/particle_system.h
)


add_executable(SwordGL
        src/main.cc
        ${IMGUI_SOURCES}
        ${IMGUI_BACKENDS}
)

target_sources(SwordGL PRIVATE
        src/main.cc
        src/core/sword_app.cc
        src/graphics/gpu_context.cc
        src/graphics/renderer.cc
        # demo
        ${IMGUI_DIR}/imgui_demo.cpp
)

set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE)
set(SDL_GPU ON CACHE BOOL "" FORCE)
set(SDL_METAL ON CACHE BOOL "" FORCE)
add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)

set(SDLSHADERCROSS_VENDORED ON  CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_TESTS   OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_CLI     OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/SDL_shadercross)

add_subdirectory(dependencies/GLM)

add_subdirectory(dependencies/SimpleECS)

target_include_directories(SwordGL PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

set(SDLSHADERCROSS_VENDORED ON  CACHE BOOL "" FORCE)

target_link_libraries(SwordGL PRIVATE
        SDL3::SDL3
        SDL3_shadercross-static
        glm::glm
        Simple_ECS
)

add_dependencies(SwordGL shaders)

# root directory macro
target_compile_definitions(SwordGL PRIVATE
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
