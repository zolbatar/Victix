#pragma once

#include <OpenGL/gl.h>
#include <string>
#include <cairo.h>
#include "imgui.h"

class Interface {
private:
    static float dpi;
    static float dpi_scaling;
    static int width, height;

    // Blur
    static GLuint blur_texture_h, blur_texture_v, final_texture;
    static GLuint fbo_h, fbo_v, fbo_final;
    static GLuint blur_shader_program;
    static GLuint final_shader_program;
    static GLuint vao, vbo;

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

    static double easeInOutQuad(double t, double b, double c, double d);

    static GLuint CreateFBO(GLuint texture);

    static void SetupBlur(int _width, int _height);

    static void ShutdownBlur();

    static GLuint DoBlur(GLuint cairo_texture, int _width, int _height);

    static GLuint CreateShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource);

    static GLuint CompileShader(const char *shaderSource, GLenum shaderType);

    static void RenderQuad();

};
