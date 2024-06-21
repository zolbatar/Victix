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
pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
find_package(Stb REQUIRED)

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
        src/ui/CairoDebugDraw.h
        src/ui/CairoDebugDraw.h
        src/objects/Object.cpp
        src/objects/Object.h
        src/objects/Generic.cpp
        src/objects/Generic.h
        src/objects/Emplacement.cpp
        src/objects/Emplacement.h
        src/ui/Minimap.cpp
        src/ui/Minimap.h
        src/objects/Types.h
        src/objects/Player.h
        src/ui/Shaders.h)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${Stb_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/imgui)

target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        box2d::box2d
        PkgConfig::cairo
        Freetype::Freetype
        glfw)