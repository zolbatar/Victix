#include <iostream>
#include <string>
#include "Interface.h"
#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

float Interface::dpi = 72.0f;
float Interface::dpi_scaling = 1.0;
int Interface::width;
int Interface::height;

GLuint Interface::blur_texture_h, Interface::blur_texture_v, Interface::final_texture;
GLuint Interface::fbo_h, Interface::fbo_v, Interface::fbo_final;
GLuint Interface::blur_shader_program;
GLuint Interface::final_shader_program;

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

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

cairo_pattern_t *Interface::SetLinear(double cx, double cy, double length, double degrees) {
    // Define the angle in radians
    double angle = degrees * 3.14159265358979323846 / 180.0;

    // Calculate the start and end points of the gradient
    double x1 = cx - length * cos(angle) / 2;
    double y1 = cy - length * sin(angle) / 2;
    double x2 = cx + length * cos(angle) / 2;
    double y2 = cy + length * sin(angle) / 2;

    // Create a linear gradient pattern at the specified angle
    return cairo_pattern_create_linear(x1, y1, x2, y2);
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
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

const char *blur_vertex_shader_src = R"(
    #version 330 core
    layout (location = 0) in vec2 inPos;
    layout (location = 1) in vec2 inTexCoord;

    out vec2 TexCoord;

    void main() {
        gl_Position = vec4(inPos, 0.0, 1.0);
        TexCoord = inTexCoord;
    }
)";

const char *blur_fragment_shader_src = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D image;
    uniform bool horizontal;
    uniform float weight[5];

    void main() {
        vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
        vec3 result = texture(image, TexCoord).rgb * weight[0]; // current fragment's contribution

        if (horizontal) {
            for (int i = 1; i < 5; ++i) {
                result += texture(image, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                result += texture(image, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            }
        } else {
            for (int i = 1; i < 5; ++i) {
                result += texture(image, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                result += texture(image, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            }
        }

        FragColor = vec4(result, 1.0);
    }
)";

const char *final_fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D horizontalBlurredImage;
        uniform sampler2D verticalBlurredImage;
        void main() {
            //vec3 color = texture(horizontalBlurredImage, TexCoord).rgb + texture(verticalBlurredImage, TexCoord).rgb;
            vec3 color = texture(horizontalBlurredImage, TexCoord).rgb;
            FragColor = vec4(color, 1.0);
        }
    )";

void Interface::SetupBlur(int _width, int _height) {
    glGenTextures(1, &blur_texture_h);
    glGenTextures(1, &blur_texture_v);
    glGenTextures(1, &final_texture);

    GLint format = GL_RGB;

    // Set up blur textures
    glBindTexture(GL_TEXTURE_2D, blur_texture_h);
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, blur_texture_v);
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Setup final texture
    glBindTexture(GL_TEXTURE_2D, final_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    fbo_h = CreateFBO(blur_texture_h);
    fbo_v = CreateFBO(blur_texture_v);
    fbo_final = CreateFBO(final_texture);

    blur_shader_program = CreateShaderProgram(blur_vertex_shader_src, blur_fragment_shader_src);
    final_shader_program = CreateShaderProgram(blur_vertex_shader_src, final_fragment_shader_source);
}

void Interface::ShutdownBlur() {
    glDeleteProgram(blur_shader_program);
    glDeleteProgram(final_shader_program);
    glDeleteTextures(1, &blur_texture_h);
    glDeleteTextures(1, &blur_texture_v);
    glDeleteTextures(1, &final_texture);
    glDeleteFramebuffers(1, &fbo_h);
    glDeleteFramebuffers(1, &fbo_v);
    glDeleteFramebuffers(1, &fbo_final);
}

GLuint Interface::DoBlur(GLuint cairo_texture) {
    static float weight[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};

    // First pass: horizontal blur
/*    glBindFramebuffer(GL_FRAMEBUFFER, fbo_h);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blur_shader_program);
    glUniform1i(glGetUniformLocation(blur_shader_program, "horizontal"), 1);
    glUniform1fv(glGetUniformLocation(blur_shader_program, "weight"), 5, weight);
    glBindTexture(GL_TEXTURE_2D, cairo_texture);
    RenderQuad();

    // Second pass: vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_v);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blur_shader_program);
    glUniform1i(glGetUniformLocation(blur_shader_program, "horizontal"), 0);
    glUniform1fv(glGetUniformLocation(blur_shader_program, "weight"), 5, weight);
    glBindTexture(GL_TEXTURE_2D, cairo_texture);
    RenderQuad();*/

    // Final render: combine original and blurred textures
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo_final);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Example: Black clear color
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(final_shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blur_texture_h);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blur_texture_v);
    RenderQuad();

    return final_texture;
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

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}