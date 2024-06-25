cmake_minimum_required(VERSION 3.28)
project(Victix)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(GL_SILENCE_DEPRECATION)

message(STATUS "Vcpkg path: ${VCPKG_INSTALLED_DIR}")
message(STATUS "Triplet: ${VCPKG_TARGET_TRIPLET}")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(freetype)
find_package(box2d CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Stb REQUIRED)
find_package(unofficial-skia CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/misc/freetype/imgui_freetype.cpp
        src/imgui/backends/imgui_impl_glfw.cpp
        src/imgui/backends/imgui_impl_opengl3.cpp
        src/App.cpp
        src/App.h
        src/model/World.cpp
        src/model/World.h
        src/model/Terrain.cpp
        src/model/Terrain.h
        src/model/PerlinNoise.cpp
        src/model/PerlinNoise.h
        src/ui/Interface.cpp
        src/model/WorldPosition.h
        src/objects/Object.cpp
        src/objects/Object.h
        src/objects/Bomb.cpp
        src/objects/Bomb.h
        src/objects/Emplacement.cpp
        src/objects/Emplacement.h
        src/ui/Minimap.cpp
        src/ui/Minimap.h
        src/objects/Types.h
        src/objects/Player.h
        src/ui/Shaders.h
        src/ui/Skia.h
        src/ui/Skia.cpp
        src/model/SkiaDebugDraw.h
        src/objects/Explosion.cpp
        src/objects/Explosion.h)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${Stb_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/imgui)

target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        unofficial::skia::skia
        box2d::box2d
        Freetype::Freetype
        glfw)