#pragma once

#include <OpenGL/gl.h>
#include <string>
#include <include/core/SkPoint.h>
#include <include/core/SkPaint.h>
#include "imgui.h"

class Interface {
private:
    static float dpi;
    static float dpi_scaling;
    static int width, height;

public:
    static GLuint crt_shader;

    static void CreateShader();

    static int LastTexWidth() { return width; }

    static int LastTexHeight() { return height; }

    static float GetDPI() { return dpi; }

    static float GetDPIScaled() { return dpi * dpi_scaling; }

    static void SetDPIScaling(float _dpi_scaling) { dpi_scaling = _dpi_scaling; }

    static float GetDPIScaling() { return dpi_scaling; }

    static void RenderDropShadow(ImTextureID tex_id, ImVec2 pos, ImVec2 _size, float size, ImU8 opacity);

    static GLuint LoadTexture(const std::string &filename, GLint type);

    static GLuint CreateTexture(int width, int height, GLint type, const GLvoid *pixels);

    static double easeInOutQuad(double t, double b, double c, double d);

    static GLuint CreateFBO(GLuint texture);

    static GLuint CreateShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource);

    static GLuint CompileShader(const char *shaderSource, GLenum shaderType);

    static void RenderQuad();

    static void DrawArrowLine(SkPoint start, SkPoint end, float arrowSize, SkPaint paint);
};
