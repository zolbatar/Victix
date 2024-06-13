#pragma once

#include <GLFW/glfw3.h>

class App {
private:
    GLFWwindow *window;
public:
    App(GLFWwindow *window);

    void Go();
};
