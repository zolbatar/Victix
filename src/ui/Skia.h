#pragma once

#include <iostream>
#include <random>
#include <fstream>
#include "include/core/SkColorSpace.h"
#include "include/core/SkColorType.h"
#include "include/core/SkData.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/core/SkPathEffect.h"
#include "include/effects/SkBlurMaskFilter.h"
#include "include/effects/SkGradientShader.h"
#include "include/encode/SkPngEncoder.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "../model/WorldPosition.h"
#include "include/utils/SkShadowUtils.h"

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

class Skia {
private:
    GLuint textureID;
    GrDirectContext *context;
    static SkSurface *surface;
    static long frame;

public:
    Skia();

    static long GetFrame() { return frame; }

    static void StartFrame();

    void MakeFrame(WorldPosition &state);

    void EndFrame();

    static SkCanvas *GetCanvas() { return surface->getCanvas(); }

    static ImVec2 Reverse(float x, float y);
};
