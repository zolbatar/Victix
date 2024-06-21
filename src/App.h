#pragma once

#include <GLFW/glfw3.h>
#include <cairo.h>
#include "ui/Interface.h"
#include "ui/Skia.h"

class App {
private:
    GLFWwindow *window;
    GLFWmonitor *primary;
    const GLFWvidmode *mode;
    bool show_demo_window = false;
    cairo_surface_t *surface;
    cairo_t *cr;
    GLuint render;
    GLuint bg;
    const float ui_font_size = 24.0f;
    std::unique_ptr<Skia> skia;

public:
    App(GLFWwindow *window);
    ~App();

    void Go();
};
