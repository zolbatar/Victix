cmake_minimum_required(VERSION 3.28)
project(Victix)

set(CMAKE_CXX_STANDARD 17)

find_package(opengl_system)
find_package(glfw3)
find_package(freetype)
find_package(box2d)
find_package(cairo)
find_package(stb)

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
        src/ui/Interface.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/imgui)

target_link_libraries(${PROJECT_NAME}
        opengl::opengl
        box2d::box2d
        cairo::cairo
        Freetype::Freetype
        stb::stb
        glfw)