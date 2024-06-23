#include "Skia.h"
#include "Interface.h"
#include "../model/Terrain.h"

SkSurface *Skia::surface;

extern GLFWwindow *window;
extern int window_width;
extern int window_height;
extern int width, height;
extern std::unique_ptr<Terrain> terrain;

std::random_device rd;  // Random device to seed the generator
std::mt19937 gen(rd()); // Standard Mersenne Twister engine seeded with rd()

Skia::Skia() {
    // Initialize Skia's OpenGL interface
    auto interface = GrGLMakeNativeInterface();
    if (!interface) {
        fprintf(stderr, "Failed to create Skia OpenGL interface.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }

    context = sk_sp<GrDirectContext>(GrDirectContexts::MakeGL(interface)).release();
    if (!context) {
        fprintf(stderr, "Failed to create Skia OpenGL context.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }

    // Create texture
    SkImageInfo imageInfo = SkImageInfo::Make(width, height,
                                              kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    surface = SkSurfaces::RenderTarget(context, skgpu::Budgeted::kNo, imageInfo).release();

    // Get texture ID
    auto backendTexture = SkSurfaces::GetBackendTexture(surface,
                                                        SkSurfaces::BackendHandleAccess::kFlushWrite);
    GrGLTextureInfo textureInfo;
    GrBackendTextures::GetGLTextureInfo(backendTexture, &textureInfo);
    textureID = textureInfo.fID;

    if (!surface) {
        printf("Could not make surface from GL DirectContext\n");
        exit(1);
    }
}

void Skia::MakeFrame(WorldPosition &state) {
    std::uniform_real_distribution<> x(0.0, width);
    std::uniform_real_distribution<> y(0.0, height);

    SkCanvas *canvas = surface->getCanvas();

    // Draw something onto the Skia surface
    canvas->clear(SkColorSetARGB(255, 0, 0, 0));

    // Gradient fill
    SkColor colors[] = {
//            SkColorSetARGB(255, 55, 16, 102),
            SkColorSetARGB(255, 255, 69, 157),
            SkColorSetARGB(255, 255, 144, 0)
    };
    SkScalar colorPositions[] = {0.0f/*, 0.5f*/, 1.0f};
    SkPoint points[] = {SkPoint::Make(0, 0), SkPoint::Make(0, height)};
    sk_sp<SkShader> shader = SkGradientShader::MakeLinear(points, colors, colorPositions, 2, SkTileMode::kClamp);

    // Apply the shader to a paint object
    SkPaint paint;
    paint.setShader(shader);

    // Draw a rectangle with the gradient on the canvas
    canvas->drawRect(SkRect::MakeLTRB(0, 0, width, height), paint);

    // Force draw.....WHYYY
    paint.reset();
    paint.setColor(SkColorSetARGB(255, 0, 0, 0));
    paint.setStrokeWidth(5.0f);
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 0.05f));
    canvas->drawLine(0, 0, width, 1, paint);
}

void Skia::EndFrame() {

    // Flush Skia commands
    context->flushAndSubmit();

    // Assume shaderProgram is the compiled and linked shader program
/*    glUseProgram(Interface::crt_shader);
    glUniform1i(glGetUniformLocation(Interface::crt_shader, "screenTexture"), textureID); // Texture unit 0
    glUniform1f(glGetUniformLocation(Interface::crt_shader, "time"), glfwGetTime());*/

    // Render
    ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(textureID),
            ImVec2(0.0f, 0.0f),
            ImVec2(width, height),
            ImVec2(0.0f, 0.0f),
            ImVec2(Interface::GetDPIScaling(), Interface::GetDPIScaling()));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skia::StartFrame() {
    ImGuiIO &io = ImGui::GetIO();
    SkMatrix matrix;
    matrix.setScale(1, -1);
    matrix.postTranslate(0, io.DisplaySize.y * Interface::GetDPIScaling());
    surface->getCanvas()->resetMatrix();
    surface->getCanvas()->concat(matrix);
}
