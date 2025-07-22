#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <iostream>
#include "thirdparty/tinyfiledialogs/tinyfiledialogs.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstdint>
namespace fs = std::filesystem;

// Убедимся, что ImTextureID совместим с GLuint
static_assert(sizeof(ImTextureID) >= sizeof(GLuint), "ImTextureID too small for GLuint");

GLuint LoadTextureFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture_id;
}

struct IconTextures {
    GLuint play;
    GLuint pause;
    GLuint prev;
    GLuint next;
} icons;

// Структура для темы
struct CustomTheme {
    ImVec4 windowBg;
    ImVec4 childBg;
    ImVec4 text;
    ImVec4 button;
    ImVec4 buttonHovered;
    ImVec4 buttonActive;
    ImVec4 frameBg;
    ImVec4 sliderGrab;
    ImVec4 sliderGrabActive;
    ImVec4 accent;
};

ImVec4 HexToImVec4(const std::string& hexColor) {
    if (hexColor.empty() || hexColor[0] != '#') return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    
    unsigned int hexValue;
    sscanf(hexColor.c_str(), "#%x", &hexValue);
    
    return ImVec4(
        ((hexValue >> 16) & 0xFF) / 255.0f,
        ((hexValue >> 8) & 0xFF) / 255.0f,
        ((hexValue) & 0xFF) / 255.0f,
        1.0f
    );
}

void ApplyCustomTheme(const CustomTheme& theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.Colors[ImGuiCol_WindowBg] = theme.windowBg;
    style.Colors[ImGuiCol_ChildBg] = theme.childBg;
    style.Colors[ImGuiCol_Text] = theme.text;
    style.Colors[ImGuiCol_Button] = theme.button;
    style.Colors[ImGuiCol_ButtonHovered] = theme.buttonHovered;
    style.Colors[ImGuiCol_ButtonActive] = theme.buttonActive;
    style.Colors[ImGuiCol_FrameBg] = theme.frameBg;
    style.Colors[ImGuiCol_SliderGrab] = theme.sliderGrab;
    style.Colors[ImGuiCol_SliderGrabActive] = theme.sliderGrabActive;
    
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(6, 4);
    style.WindowRounding = 4;
    style.FrameRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 4;
}

bool IconButton(const char* id, GLuint texture_id, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    bool clicked = ImGui::ImageButton(
        id,
        (ImTextureID)(intptr_t)texture_id,
        size,
        ImVec2(0.0f, 0.0f), 
        ImVec2(1.0f, 1.0f)
    );
    ImGui::PopStyleColor();
    return clicked;
}

void ShowThemeEditor(CustomTheme& theme) {
    static char windowBgHex[8] = "#1d2021";
    static char childBgHex[8] = "#282828";
    static char textHex[8] = "#ebdbb2";
    static char buttonHex[8] = "#504945";
    static char buttonHoveredHex[8] = "#7c6f64";
    static char buttonActiveHex[8] = "#a89984";
    static char frameBgHex[8] = "#504945";
    static char sliderGrabHex[8] = "#d79921";
    static char sliderGrabActiveHex[8] = "#fabd2f";
    static char accentHex[8] = "#fabd2f";
    
    static bool showThemeEditor = false;
    
    if (ImGui::Button("Theme Settings")) {
        showThemeEditor = !showThemeEditor;
    }
    
    if (showThemeEditor) {
        ImGui::Begin("Theme Editor", &showThemeEditor, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("Enter hex colors (e.g. #1d2021)");
        ImGui::Separator();
        
        ImGui::InputText("Window BG", windowBgHex, 8);
        ImGui::InputText("Child BG", childBgHex, 8);
        ImGui::InputText("Text", textHex, 8);
        ImGui::InputText("Button", buttonHex, 8);
        ImGui::InputText("Button Hovered", buttonHoveredHex, 8);
        ImGui::InputText("Button Active", buttonActiveHex, 8);
        ImGui::InputText("Frame BG", frameBgHex, 8);
        ImGui::InputText("Slider Grab", sliderGrabHex, 8);
        ImGui::InputText("Slider Active", sliderGrabActiveHex, 8);
        ImGui::InputText("Accent", accentHex, 8);
        
        if (ImGui::Button("Apply Theme")) {
            theme.windowBg = HexToImVec4(windowBgHex);
            theme.childBg = HexToImVec4(childBgHex);
            theme.text = HexToImVec4(textHex);
            theme.button = HexToImVec4(buttonHex);
            theme.buttonHovered = HexToImVec4(buttonHoveredHex);
            theme.buttonActive = HexToImVec4(buttonActiveHex);
            theme.frameBg = HexToImVec4(frameBgHex);
            theme.sliderGrab = HexToImVec4(sliderGrabHex);
            theme.sliderGrabActive = HexToImVec4(sliderGrabActiveHex);
            theme.accent = HexToImVec4(accentHex);
            
            ApplyCustomTheme(theme);
        }
        
        ImGui::End();
    }
}

void ShowMainInterface(CustomTheme& theme, GLuint my_texture, const ImVec2& image_size) {
    static bool isPlaying = false;
    static float progress = 0.65f;
    static std::vector<std::string> loadedFiles;
    static std::unordered_set<std::string> supportedFormats = {".mp3", ".wav", ".ogg"};
    static bool showLoadedFiles = false;
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(windowSize);
    
    ImGui::Begin("MainWindow", nullptr, 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse
    );

    // Кнопка настроек темы
    ImGui::SetCursorPos(ImVec2(windowSize.x - 120, 15));
    ShowThemeEditor(theme);

    ImGui::SetCursorPos(ImVec2(20, 15));
    ImGui::TextColored(theme.accent, "CatMp3");

    ImGui::SetCursorPos(ImVec2(15, 60));
    ImGui::BeginGroup();

    // Левая панель (плейлисты)
    ImGui::BeginChild("LeftPanel", ImVec2(windowSize.x * 0.20f, windowSize.y - 80), true);
    {
        ImGui::TextColored(theme.accent, "Playlists");
        ImGui::Separator();
        
        if (showLoadedFiles) {
            ImGui::BeginChild("File List", ImVec2(0, 300), true);
            for (const auto& file : loadedFiles) {
                ImGui::Selectable(file.c_str(), false);
            }
            ImGui::EndChild();
        }
        
        if (ImGui::Button("Add Playlist", ImVec2(-1, 0))) {
            const char* folderPath = tinyfd_selectFolderDialog("Select Folder", nullptr);
            if (folderPath) {
                loadedFiles.clear();
                for (const auto& entry : fs::directory_iterator(folderPath)) {
                    std::string ext = entry.path().extension().string();
                    if (entry.is_regular_file() && supportedFormats.count(ext)) {
                        loadedFiles.push_back(entry.path().filename().string());
                    }
                }
                showLoadedFiles = true;
            }
        }
    }
    ImGui::EndChild();
    
    ImGui::EndGroup();
    ImGui::SameLine();

    // Правая панель (плеер)
    ImGui::BeginGroup();
    {
        // Область с обложкой
        ImGui::BeginChild("MainPanel", ImVec2(0, 380), true);
        {   
            const float cover_size = 200.0f;
            ImVec2 center_pos = ImVec2(
                (ImGui::GetContentRegionAvail().x - cover_size) * 0.5f,
                (ImGui::GetContentRegionAvail().y - cover_size - 80) * 0.3f
            );
            
            // Обложка альбома
            ImGui::SetCursorPos(center_pos);
            
             if (my_texture != 0) {
            // КОРРЕКТНОЕ приведение типов:
            ImTextureID tex_id = (ImTextureID)(intptr_t)my_texture;
            ImGui::Image(tex_id, image_size);
        } else {
            ImGui::TextColored(ImVec4(1,0,0,1), "Texture not loaded!");
        }
        
            
            // Информация о треке
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 200) * 0.6f);
            ImGui::SetCursorPosY(center_pos.y + cover_size + 30);
            
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 150) * 0.5f);
            
            ImGui::SetCursorPosX(30);
            ImGui::SetCursorPosY(center_pos.y + cover_size + 70);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60);
            ImGui::SliderFloat("##progress", &progress, 0.0f, 1.0f, "");
            
            // Время трека
            ImGui::SetCursorPosX(30);
            ImGui::Text("0:00");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 50);
            ImGui::Text("0:00");
        }
        ImGui::EndChild();

        // Панель управления
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
        ImGui::BeginChild("Controls", ImVec2(0, 105), true);
        {
            const float buttonWidth = 50.0f;
            const float playButtonWidth = 60.0f;
            const float buttonHeight = 50.0f;
            const float totalWidth = buttonWidth * 2 + playButtonWidth + ImGui::GetStyle().ItemSpacing.x * 2;
            const float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
            ImGui::SetCursorPosX(startX);
            
            if (IconButton("prev", icons.prev, ImVec2(buttonWidth, buttonHeight))) {
                // Previous track
            }
            
            ImGui::SameLine();
            
            if (IconButton("play", isPlaying ? icons.pause : icons.play, ImVec2(playButtonWidth, playButtonWidth))) {
                isPlaying = !isPlaying;
            }
            
            ImGui::SameLine();
            
            if (IconButton("next", icons.next, ImVec2(buttonWidth, buttonHeight))) {
                // Next track
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();

    ImGui::End();
}

int main() {
    if (!glfwInit()) return 1;

    GLFWwindow* window = glfwCreateWindow(900, 580, "CatMp3", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Инициализация темы по умолчанию (Gruvbox)
    CustomTheme theme;
    theme.windowBg = HexToImVec4("#1d2021");
    theme.childBg = HexToImVec4("#282828");
    theme.text = HexToImVec4("#ebdbb2");
    theme.button = HexToImVec4("#504945");
    theme.buttonHovered = HexToImVec4("#7c6f64");
    theme.buttonActive = HexToImVec4("#a89984");
    theme.frameBg = HexToImVec4("#504945");
    theme.sliderGrab = HexToImVec4("#d79921");
    theme.sliderGrabActive = HexToImVec4("#fabd2f");
    theme.accent = HexToImVec4("#fabd2f");
    
    ApplyCustomTheme(theme);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");



    GLuint my_texture = LoadTextureFromFile("example.jpg");
    ImVec2 image_size(300.0f, 200.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowMainInterface(theme, my_texture, image_size);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(
            theme.windowBg.x * theme.windowBg.w,
            theme.windowBg.y * theme.windowBg.w,
            theme.windowBg.z * theme.windowBg.w,
            theme.windowBg.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Очистка текстур
    glDeleteTextures(1, &icons.play);
    glDeleteTextures(1, &icons.pause);
    glDeleteTextures(1, &icons.prev);
    glDeleteTextures(1, &icons.next);
    if (my_texture != 0) glDeleteTextures(1, &my_texture);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}