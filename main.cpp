#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <cmath>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tinyfiledialogs.h"

#define ICON_FA_PLAY u8"\u25B6"
#define ICON_FA_PAUSE u8"\u23F8"
#define ICON_FA_STEP_BACKWARD u8"\u23EE"
#define ICON_FA_STEP_FORWARD u8"\u23ED"
#define ICON_FA_MUSIC u8"\U0001F3B5"
#define ICON_FA_LIST u8"\u2630"
#define ICON_FA_PLUS u8"\u002B"
#define ICON_FA_CLOSE u8"\u2715"
#define ICON_FA_MINIMIZE u8"\u2013"
#define ICON_FA_MAXIMIZE u8"\u26F6"

int main() {
    if (!glfwInit()) return 1;
    

    glfwWindowHint(GLFW_SAMPLES, 4);
    
    GLFWwindow* window = glfwCreateWindow(900, 580, "CatMp3", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImFont* mainFont = io.Fonts->AddFontFromFileTTF("Manrope-Regular.ttf", 16.0f);
    ImFont* mediumFont = io.Fonts->AddFontFromFileTTF("Manrope-Medium.ttf", 18.0f);
    ImFont* boldFont = io.Fonts->AddFontFromFileTTF("Manrope-Bold.ttf", 24.0f);
    ImFont* titleFont = io.Fonts->AddFontFromFileTTF("Manrope-ExtraBold.ttf", 32.0f);
    
    io.Fonts->Build();
    io.FontDefault = mainFont;
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    
    ImGui::StyleColorsLight();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.70f, 0.78f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.75f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.70f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.75f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.70f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.95f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.92f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.88f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
    
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(4, 4);
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool isPlaying = false;
    float progress = 0.65f;
    bool windowHovered = false;
    ImVec2 dragStart;
    bool isDragging = false;

    while (!glfwWindowShouldClose(window)) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::IsMouseDown(0) && windowHovered) {
            if (!isDragging) {
                isDragging = true;
                int winX, winY;
                glfwGetWindowPos(window, &winX, &winY);
                ImVec2 mousePos = ImGui::GetMousePos();
                dragStart = ImVec2(winX - mousePos.x, winY - mousePos.y);
            }
            if (isDragging) {
                ImVec2 mousePos = ImGui::GetMousePos();
                glfwSetWindowPos(window, mousePos.x + dragStart.x, mousePos.y + dragStart.y);
            }
        } else {
            isDragging = false;
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        
        ImGui::Begin("MainWindow", nullptr, 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );
        
        windowHovered = ImGui::IsWindowHovered();

        ImGui::SetCursorPos(ImVec2(20, 15));
        ImGui::PushFont(titleFont);
        ImGui::Text("CatMp3");
        ImGui::PopFont();

        ImGui::SetCursorPos(ImVec2(15, 60));
        ImGui::BeginGroup();

        ImGui::BeginChild("LeftPanel", ImVec2(width * 0.22f, height - 80), true);
        {
            ImGui::PushFont(mediumFont);
            ImGui::Text("Playlists");
            ImGui::PopFont();
            ImGui::Separator();
            
            if (ImGui::Button(ICON_FA_PLUS " Add Playlist", ImVec2(-1, 0))) {
                const char* folderPath = tinyfd_selectFolderDialog(
                    "kkkk",
                    nullptr
                );
                if(folderPath){
                    static std::string selectedFolder;
                    selectedFolder = folderPath;
                }
            }
        }
        ImGui::EndChild();
        
        ImGui::EndGroup();
        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            ImGui::BeginChild("MainPanel", ImVec2(0, 380), true);
            {   
                const float cover_size = 200.0f;
                ImVec2 center_pos = ImVec2(
                    (ImGui::GetContentRegionAvail().x - cover_size) * 0.5f,
                    (ImGui::GetContentRegionAvail().y - cover_size - 80) * 0.3f
                );
                
                ImGui::SetCursorPos(center_pos);
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + cover_size, p0.y + cover_size);
                draw_list->AddRectFilled(p0, p1, IM_COL32(230, 230, 230, 255));
                
                ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 200) * 0.6f);
                ImGui::SetCursorPosY(center_pos.y + cover_size + 30);
                ImGui::PushFont(mediumFont);
                ImGui::Text("Prince");
                ImGui::PopFont();
                
                ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 150) * 0.5f);
                ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "Ya ystal");
                
                ImGui::SetCursorPosX(30);
                ImGui::SetCursorPosY(center_pos.y + cover_size + 70);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60);
                ImGui::SliderFloat("##progress", &progress, 0.0f, 1.0f, "");
                
                ImGui::SetCursorPosX(30);
                ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "2:45");
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 50);
                ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "4:20");
            }
            ImGui::EndChild();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
            ImGui::BeginChild("SwitchingArea", ImVec2(0, 90), true);
            {
                float buttonWidth = 50.0f;
                float playButtonWidth = 60.0f;
                float buttonHeight = 50.0f;
                float totalWidth = buttonWidth * 2 + playButtonWidth + style.ItemSpacing.x * 2;
                float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
                
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
                ImGui::SetCursorPosX(startX);
                
                if (ImGui::Button(ICON_FA_STEP_BACKWARD, ImVec2(buttonWidth, buttonHeight))) {
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button(isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY, ImVec2(playButtonWidth, playButtonWidth))) {
                    isPlaying = !isPlaying;
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(buttonWidth, buttonHeight))) {
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndGroup();

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, width, height);
        glClearColor(0.95f, 0.95f, 0.96f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
