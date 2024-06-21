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
    canvas->clear(SkColorSetARGB(0, 0, 0, 255));

    SkPaint paint;
    for (int i = 0; i < 1000; i++) {
        paint.reset();
        paint.setColor(SkColorSetARGB(0xFF,
                                      rand() % 0xFF,
                                      rand() % 0xFF,
                                      rand() % 0xFF));
        paint.setStrokeWidth(5.0f);
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kSolid_SkBlurStyle, 1.0f));
//            paint.setMaskFilter(SkMaskFilter::MakeBlur(SkBlurStyle::kInner_SkBlurStyle, 2.0f));
        canvas->drawLine(x(gen),
                         y(gen),
                         x(gen),
                         y(gen),
                         paint);
    }

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
