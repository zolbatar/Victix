#include "App.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "model/World.h"

ImVec4 HexToImVec4(uint32_t hex);

App::App(GLFWwindow *window) : window(window) {
    ImGuiIO &io = ImGui::GetIO();

    // Query default monitor resolution
    primary = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(primary);

    // Load font with DPI scaling
    float xScale, yScale;
    glfwGetMonitorContentScale(primary, &xScale, &yScale);
    float dpi_scaling = xScale; // assuming you're on a system where horizontal and vertical scales are the same
    Interface::SetDPIScaling(dpi_scaling);
    io.FontGlobalScale = 1.0f / dpi_scaling;

    // Set default UI font
    io.Fonts->AddFontFromFileTTF(
            "assets/fonts/Inter-Regular.ttf",
            dpi_scaling * ui_font_size,
            nullptr,
            nullptr);

    // And build atlases
    io.Fonts->Build();

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Cairo render surface
    width = mode->width * Interface::GetDPIScaling();
    height = mode->height * Interface::GetDPIScaling();
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create(surface);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
//    cairo_scale(cr, 1.0, 1.0);
    render = Interface::CreateTexture(width, height, GL_NEAREST, nullptr);
}

void App::Go() {
    World world;

    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full screen window
        ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, HexToImVec4(0x000000FF));
        ImGui::Begin("Surface", NULL,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Render world
        world.Render(cr);

        // Write to texture and blit
        cairo_surface_flush(surface);
        glBindTexture(GL_TEXTURE_2D, render);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                     0, GL_BGRA, GL_UNSIGNED_BYTE,
                     cairo_image_surface_get_data(surface));
        ImGui::GetWindowDrawList()->AddImage(
                reinterpret_cast<ImTextureID>(render),
                ImVec2(0.0f, 0.0f),
                ImVec2(width, height),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                IM_COL32(255, 255, 255, 255));

        ImGui::End();
        ImGui::PopStyleColor();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

App::~App() {
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

ImVec4 HexToImVec4(uint32_t hex) {
    float r = ((hex >> 24) & 0xFF) / 255.0f;
    float g = ((hex >> 16) & 0xFF) / 255.0f;
    float b = ((hex >> 8) & 0xFF) / 255.0f;
    float a = (hex & 0xFF) / 255.0f;
    return ImVec4(r, g, b, a);
}