#include <string>
#include "Interface.h"
#include "imgui_internal.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

float Interface::dpi = 72.0f;
float Interface::dpi_scaling = 1.0;
int Interface::width;
int Interface::height;

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
double easeInOutQuad(double t, double b, double c, double d) {
    t /= d / 2;
    if (t < 1) {
        return c / 2 * std::pow(t, 2) + b;
    }
    t--;
    return -c / 2 * (t * (t - 2) - 1) + b;
}
