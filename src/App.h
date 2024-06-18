#pragma once

#include <GLFW/glfw3.h>
#include <cairo.h>
#include "ui/Interface.h"

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
    int width, height;
    const float ui_font_size = 18.0f;

public:
    App(GLFWwindow *window);
    ~App();

    void Go();
};
