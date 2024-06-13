cmake_minimum_required(VERSION 3.28)
project(Victix)

set(CMAKE_CXX_STANDARD 17)

find_package(opengl_system)
find_package(glfw3)
find_package(freetype)
find_package(box2d)

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
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/imgui)

target_link_libraries(${PROJECT_NAME}
        opengl::opengl
        box2d::box2d
        Freetype::Freetype
        glfw)