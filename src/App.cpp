#include "App.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "model/World.h"

ImVec4 HexToImVec4(uint32_t hex);

App::App(GLFWwindow *window) : window(window) {
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

        world.Render();

        ImGui::End();
        ImGui::PopStyleColor();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
/*        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);*/
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

ImVec4 HexToImVec4(uint32_t hex) {
    float r = ((hex >> 24) & 0xFF) / 255.0f;
    float g = ((hex >> 16) & 0xFF) / 255.0f;
    float b = ((hex >> 8) & 0xFF) / 255.0f;
    float a = (hex & 0xFF) / 255.0f;
    return ImVec4(r, g, b, a);
}