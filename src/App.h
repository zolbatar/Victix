#pragma once

#include <GLFW/glfw3.h>

class App {
private:
    GLFWwindow *window;
    bool show_demo_window;

public:
    App(GLFWwindow *window);

    void Go();
};
