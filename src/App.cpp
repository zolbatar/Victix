#include <chrono>
#include <OpenGL/gl.h>
#include "App.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "model/World.h"
#include "ui/Orbitron-Black.h"
#include "ui/Orbitron-Regular.h"
#include "ui/dosis.h"
#include "ui/Dosis-Bold.h"
#include "ui/michromo.h"

std::unique_ptr<World> game_world = nullptr;
ImFont *font_regular;
ImFont *font_bold;
extern int window_width;
extern int window_height;
int width, height;

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
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
/*    font_regular = io.Fonts->AddFontFromMemoryTTF(Orbitron_Regular_ttf, Orbitron_Regular_ttf_len,
                                                  dpi_scaling * ui_font_size, &fontConfig);
    font_bold = io.Fonts->AddFontFromMemoryTTF(Orbitron_Black_ttf, Orbitron_Black_ttf_len, dpi_scaling * ui_font_size,
                                               &fontConfig);*/
    font_regular = io.Fonts->AddFontFromMemoryTTF(Dosis_Regular_ttf, Dosis_Regular_ttf_len,
                                                  dpi_scaling * ui_font_size, &fontConfig);
    font_bold = io.Fonts->AddFontFromMemoryTTF(Dosis_Bold_ttf, Dosis_Bold_ttf_len,
                                                  dpi_scaling * ui_font_size, &fontConfig);
/*    font_regular = io.Fonts->AddFontFromMemoryTTF(Michroma_Regular_ttf, Michroma_Regular_ttf_len,
                                                  dpi_scaling * ui_font_size, &fontConfig);*/
//    font_bold = font_regular;

    // And build atlases
    io.Fonts->Build();

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Render surface size
    width = window_width * Interface::GetDPIScaling();
    height = window_height * Interface::GetDPIScaling();

    // Skia
    skia = std::make_unique<Skia>();
}

void App::Go() {
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

        // Build?
        if (game_world == nullptr) {
            ImGuiIO &io = ImGui::GetIO();
            float scale = io.DisplaySize.y / (Terrain::F_TERRAIN_HEIGHT * 3);
            game_world = std::make_unique<World>(scale);
            game_world->Build(1);
        }

        // Full screen window
        ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::Begin("Surface", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar);

        // Do SKIA!!
        skia->MakeFrame(game_world->GetState());

        // Update things, process input etc.
        if (game_world != nullptr)
            game_world->Process();

        // Render world
        game_world->PreRender(width, height);
        skia->EndFrame();

        ImGui::End();
        ImGui::PopStyleVar(2);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0,0.0,0.0,0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

App::~App() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

ImVec4 HexToImVec4(uint32_t hex) {
    float r = ((hex >> 24) & 0xFF) / 255.0f;
    float g = ((hex >> 16) & 0xFF) / 255.0f;
    float b = ((hex >> 8) & 0xFF) / 255.0f;
    float a = (hex & 0xFF) / 255.0f;
    return {r, g, b, a};
}
