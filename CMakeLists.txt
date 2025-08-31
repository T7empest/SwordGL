cmake_minimum_required(VERSION 3.31)
project(SwordGL)

set(CMAKE_CXX_STANDARD 23)

# set the output directory for built objects.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(SwordGL
        src/graphics/renderer.cc
        src/graphics/renderer.h
        src/scenes/scene.h
        src/graphics/render_passes/render_pass.h
        src/graphics/render_passes/present_pass.h
        src/graphics/render_passes/copy_pass.h
)

target_sources(SwordGL PRIVATE
        src/main.cc
        src/core/sword_app.cc
        src/graphics/gpu_context.cc
)

# Force/seed SDL options *before* add_subdirectory so they stick in the cache.
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)           # or OFF for static
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE)
set(SDL_GPU ON CACHE BOOL "" FORCE)              # <— enable SDL_gpu
set(SDL_METAL ON CACHE BOOL "" FORCE)            # <— ensure Metal video backend

set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)

add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)

target_include_directories(SwordGL PRIVATE
)

target_link_directories(SwordGL PRIVATE
)

target_link_libraries(SwordGL PRIVATE
        SDL3::SDL3
)

# root directory macro
target_compile_definitions(SwordGL PRIVATE
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
