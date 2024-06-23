#include <iostream>
#include <vector>
#include <string>
#include "Interface.h"
#include "imgui.h"
#include "Shaders.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

extern char *glsl_version_init;
float Interface::dpi = 72.0f;
float Interface::dpi_scaling = 1.0;
int Interface::width;
int Interface::height;
GLuint Interface::crt_shader;

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

void Interface::CreateShader() {
    crt_shader = CreateShaderProgram(crt_vertex_shader, crt_fragment_shader);
}

void Interface::RenderDropShadow(ImTextureID tex_id, ImVec2 pos, ImVec2 _size, float size, ImU8 opacity) {
    ImVec2 p = pos;
    ImVec2 s = _size;
    ImVec2 m = {p.x + s.x, p.y + s.y};
    float uv0 = 0.0f;      // left/top region
    float uv1 = 0.333333f; // leftward/upper region
    float uv2 = 0.666666f; // rightward/lower region
    float uv3 = 1.0f;      // right/bottom region
    ImU32 col = (opacity << 24) | 0xFFFFFF;
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddImage(tex_id, {p.x - size, p.y - size}, {p.x, p.y}, {uv0, uv0}, {uv1, uv1}, col);
    dl->AddImage(tex_id, {p.x, p.y - size}, {m.x, p.y}, {uv1, uv0}, {uv2, uv1}, col);
    dl->AddImage(tex_id, {m.x, p.y - size}, {m.x + size, p.y}, {uv2, uv0}, {uv3, uv1}, col);
    dl->AddImage(tex_id, {p.x - size, p.y}, {p.x, m.y}, {uv0, uv1}, {uv1, uv2}, col);
    dl->AddImage(tex_id, {m.x, p.y}, {m.x + size, m.y}, {uv2, uv1}, {uv3, uv2}, col);
    dl->AddImage(tex_id, {p.x - size, m.y}, {p.x, m.y + size}, {uv0, uv2}, {uv1, uv3}, col);
    dl->AddImage(tex_id, {p.x, m.y}, {m.x, m.y + size}, {uv1, uv2}, {uv2, uv3}, col);
    dl->AddImage(tex_id, {m.x, m.y}, {m.x + size, m.y + size}, {uv2, uv2}, {uv3, uv3}, col);
}

GLuint Interface::LoadTexture(const std::string &filename, GLint type) {

    // Load the image
    int comp;
    unsigned char *image = stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);

    GLuint texture = CreateTexture(width, height, type, image);
    printf("INTERFACE: Loaded %s, with texture %d.\n", filename.c_str(), texture);
    return texture;
}

GLuint Interface::CreateTexture(int _width, int _height, GLint type, const GLvoid *pixels) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
    printf("INTERFACE: Created texture %d.\n", texture);
    return texture;
}

// Easing function: easeInOutQuad
// t: current time (in range [0, duration])
// b: start value
// c: change in value (end value - start value)
// d: total duration
double Interface::easeInOutQuad(double t, double b, double c, double d) {
    t /= d / 2;
    if (t < 1) {
        return c / 2 * std::pow(t, 2) + b;
    }
    t--;
    return -c / 2 * (t * (t - 2) - 1) + b;
}

GLuint Interface::CreateFBO(GLuint texture) {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("OpenGL error creating framebuffer\n");
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

GLuint Interface::CreateShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource) {
    GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    // Create shader program and attach shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(1);
    }

    // Clean up shaders as they are now linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint Interface::CompileShader(const char *shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

/*    const GLchar *fragment_shader_with_version[2] = {glsl_version_init, shaderSource};
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 2, fragment_shader_with_version, nullptr);
    glCompileShader(shader);*/

    // Check for compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }

    return shader;
}

void Interface::RenderQuad() {
    // Vertex data for a fullscreen quad
    float vertices[] = {
            // Positions        // Texture coords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}