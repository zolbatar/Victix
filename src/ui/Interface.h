#pragma once

#include <string>
#include "imgui.h"
#include <OpenGL/gl.h>
#include <cairo.h>
#include "backends/imgui_impl_opengl3.h"

class Interface {
private:
    static float dpi;
    static float dpi_scaling;
    static int width, height;

public:
    static int LastTexWidth() { return width; }

    static int LastTexHeight() { return height; }

    static float GetDPI() { return dpi; }

    static float GetDPIScaled() { return dpi * dpi_scaling; }

    static void SetDPIScaling(float _dpi_scaling) { dpi_scaling = _dpi_scaling; }

    static float GetDPIScaling() { return dpi_scaling; }

    static void RenderDropShadow(ImTextureID tex_id, ImVec2 pos, ImVec2 _size, float size, ImU8 opacity);

    static GLuint LoadTexture(const std::string &filename, GLint type);

    static GLuint CreateTexture(int width, int height, GLint type, const GLvoid *pixels);

    static cairo_pattern_t *SetLinear(double cx, double cy, double length, double degrees);
};
