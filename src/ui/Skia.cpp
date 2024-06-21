#include "Skia.h"
#include "Interface.h"

extern GLFWwindow *window;
extern int window_width;
extern int window_height;
extern int width, height;

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

void Skia::MakeFrame() {
    std::uniform_real_distribution<> x(0.0, width);
    std::uniform_real_distribution<> y(0.0, height);

    SkCanvas *canvas = surface->getCanvas();

    // Draw something onto the Skia surface
    canvas->clear(SkColorSetARGB(255, 0, 0, 0));

    // Gradient fill
    SkColor colors[] = {
            SkColorSetARGB(255, 55, 16, 102),
            SkColorSetARGB(255, 55, 16, 102),
            SkColorSetARGB(255, 255, 144, 0)};
    SkScalar colorPositions[] = {0.0f, 0.25f, 1.0f};
    SkPoint points[] = {SkPoint::Make(0, height), SkPoint::Make(0, 0)};
    sk_sp<SkShader> shader = SkGradientShader::MakeLinear(points, colors, colorPositions, 3, SkTileMode::kClamp);

    // Apply the shader to a paint object
    SkPaint paint;
    paint.setShader(shader);
    paint.setAntiAlias(true);

    // Draw a rectangle with the gradient on the canvas
    canvas->drawRect(SkRect::MakeLTRB(0, 0, width, height), paint);

    for (int i = 0; i < 0; i++) {
        paint.reset();
        paint.setColor(SkColorSetARGB(0xFF,
                                      rand() % 0xFF,
                                      rand() % 0xFF,
                                      rand() % 0xFF));
        paint.setStrokeWidth(5.0f);
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 0.05f));
        canvas->drawLine(x(gen),
                         y(gen),
                         x(gen),
                         y(gen),
                         paint);
    }

    // Force draw.....WHYYY
    paint.reset();
    paint.setColor(SkColorSetARGB(255, 0, 0, 0));
    paint.setStrokeWidth(5.0f);
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 0.05f));
    canvas->drawLine(0, 0, width, 1, paint);

    // Flush Skia commands
    context->flushAndSubmit();

    // Render
    ImGui::GetWindowDrawList()->AddImage(
            reinterpret_cast<ImTextureID>(textureID),
            ImVec2(0.0f, 0.0f),
            ImVec2(width, height),
            ImVec2(0.0f, 0.0f),
            ImVec2(Interface::GetDPIScaling(), Interface::GetDPIScaling()));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
