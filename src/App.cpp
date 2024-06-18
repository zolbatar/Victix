#include "App.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "model/World.h"

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
    render = Interface::CreateTexture(width, height, GL_NEAREST, nullptr);
}

void App::Go() {
    std::unique_ptr<World> world;

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

        // Update things, process input etc.
        if (world != nullptr)
            world->Process();

        // Full screen window
        ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin("Surface", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Render world
        if (world == nullptr)
            world = std::make_unique<World>();
        world->Render(cr, surface, render, width, height);

        ImGui::End();
        ImGui::PopStyleVar();

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
    return {r, g, b, a};
}
