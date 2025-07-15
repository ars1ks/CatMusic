#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <cmath>
#include <string>
#include <filesystem>
#include <vector>
#include <mutex>
#include <thread>
#include <iostream>
#include <atomic>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "tinyfiledialogs.h"
#include "imgui_internal.h"

namespace fs = std::filesystem;

// Глобальные шрифты
ImFont* mainFont = nullptr;
ImFont* mediumFont = nullptr;
ImFont* boldFont = nullptr;
ImFont* titleFont = nullptr;

// Структура для хранения текстур иконок
struct IconTextures {
    GLuint play;
    GLuint pause;
    GLuint stop;
    GLuint prev;
    GLuint next;
    int width;
    int height;
} icons;

// Функция для создания текстуры из массива байтов
bool LoadTextureFromMemory(const unsigned char* image_data, int image_width, int image_height, GLuint* out_texture) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
    *out_texture = textureID;
    return true;
}

// Пример байт-кода для иконки play (замените на реальные данные)
static unsigned char play_icon_data[] = {
    // Здесь должен быть массив байтов PNG изображения
    // Например, сгенерированный через stb_image или другой конвертер
};

// Аналогично для других иконок
static unsigned char pause_icon_data[] = {};
static unsigned char stop_icon_data[] = {};
static unsigned char prev_icon_data[] = {};
static unsigned char next_icon_data[] = {};

// Функция загрузки ресурсов
void LoadResources() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Загрузка шрифтов
    mainFont = io.Fonts->AddFontFromFileTTF("Manrope-Regular.ttf", 16.0f);
    mediumFont = io.Fonts->AddFontFromFileTTF("Manrope-Medium.ttf", 18.0f);
    boldFont = io.Fonts->AddFontFromFileTTF("Manrope-Bold.ttf", 20.0f);
    titleFont = io.Fonts->AddFontFromFileTTF("Manrope-ExtraBold.ttf", 24.0f);
    
    if (!mainFont || !mediumFont || !boldFont || !titleFont) {
        std::cerr << "Failed to load fonts! Using default ImGui font." << std::endl;
        // Если шрифты не загрузились, используем стандартный шрифт ImGui
        mainFont = mediumFont = boldFont = titleFont = io.Fonts->AddFontDefault();
    }

    // Загрузка иконок из памяти
    int icon_width = 64, icon_height = 64; // Укажите реальные размеры ваших иконок
    LoadTextureFromMemory(play_icon_data, icon_width, icon_height, &icons.play);
    LoadTextureFromMemory(pause_icon_data, icon_width, icon_height, &icons.pause);
    LoadTextureFromMemory(stop_icon_data, icon_width, icon_height, &icons.stop);
    LoadTextureFromMemory(prev_icon_data, icon_width, icon_height, &icons.prev);
    LoadTextureFromMemory(next_icon_data, icon_width, icon_height, &icons.next);
    icons.width = icon_width;
    icons.height = icon_height;
}

// GruvBox цветовая схема
void SetupGruvboxStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Цвета
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.83f, 0.81f, 0.74f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.14f, 0.13f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.20f, 0.19f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.20f, 0.19f, 0.16f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.43f, 0.41f, 0.36f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.40f, 0.37f, 0.30f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.58f, 0.54f, 0.43f, 0.54f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.66f, 0.61f, 0.49f, 0.54f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.25f, 0.22f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.40f, 0.37f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.25f, 0.22f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.20f, 0.19f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.19f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.40f, 0.37f, 0.30f, 0.54f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.58f, 0.54f, 0.43f, 0.54f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.66f, 0.61f, 0.49f, 0.54f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.69f, 0.53f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]          = ImVec4(0.69f, 0.53f, 0.21f, 0.54f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.84f, 0.65f, 0.26f, 0.54f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.40f, 0.37f, 0.30f, 0.54f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.58f, 0.54f, 0.43f, 0.54f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.66f, 0.61f, 0.49f, 0.54f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.69f, 0.53f, 0.21f, 0.54f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.84f, 0.65f, 0.26f, 0.54f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.84f, 0.65f, 0.26f, 0.54f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.43f, 0.41f, 0.36f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.69f, 0.53f, 0.21f, 0.54f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.84f, 0.65f, 0.26f, 0.54f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.40f, 0.37f, 0.30f, 0.54f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.69f, 0.53f, 0.21f, 0.54f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.84f, 0.65f, 0.26f, 0.54f);
    style.Colors[ImGuiCol_Tab]                  = ImVec4(0.25f, 0.22f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]           = ImVec4(0.69f, 0.53f, 0.21f, 0.54f);
    style.Colors[ImGuiCol_TabActive]            = ImVec4(0.40f, 0.37f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]         = ImVec4(0.25f, 0.22f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.40f, 0.37f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.83f, 0.81f, 0.74f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.84f, 0.65f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.69f, 0.53f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.84f, 0.65f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.25f, 0.22f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]    = ImVec4(0.43f, 0.41f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight]     = ImVec4(0.43f, 0.41f, 0.36f, 0.50f);
    style.Colors[ImGuiCol_TableRowBg]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.69f, 0.53f, 0.21f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]       = ImVec4(0.84f, 0.65f, 0.26f, 0.95f);
    style.Colors[ImGuiCol_NavHighlight]         = ImVec4(0.84f, 0.65f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight]= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.19f, 0.16f, 0.73f);

    // Стили
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.CellPadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(6, 4);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 20;
    style.ScrollbarSize = 14;
    style.GrabMinSize = 12;
    
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    
    style.WindowRounding = 4;
    style.ChildRounding = 4;
    style.FrameRounding = 4;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 4;
    style.TabRounding = 4;
}

// Функция для создания кнопки с иконкой
bool IconButton(const char* id, GLuint texture_id, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
    
    bool clicked = ImGui::ImageButton(
        id,
        (ImTextureID)(intptr_t)texture_id,
        size,
        ImVec2(0.0f, 0.0f), 
        ImVec2(1.0f, 1.0f), 
        ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
    );
    
    ImGui::PopStyleColor(3);
    return clicked;
}

void ShowMainInterface() {
    static bool isPlaying = false;
    static float progress = 0.65f;
    static std::vector<std::string> loadedFiles;
    static bool showLoadedFiles = false;
    static std::mutex filesMutex;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    // Главное окно
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(windowSize);
    
    ImGui::Begin("MainWindow", nullptr, 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    // Заголовок
    ImGui::SetCursorPos(ImVec2(20, 15));
    if (titleFont) ImGui::PushFont(titleFont);
    ImGui::TextColored(ImVec4(0.84f, 0.65f, 0.26f, 1.0f), "CatMp3");
    if (titleFont) ImGui::PopFont();

    // Основная компоновка
    ImGui::SetCursorPos(ImVec2(15, 60));
    ImGui::BeginGroup();

    // Левая панель (плейлисты)
    ImGui::BeginChild("LeftPanel", ImVec2(windowSize.x * 0.22f, windowSize.y - 80), true);
    {
        if (mediumFont) ImGui::PushFont(mediumFont);
        ImGui::TextColored(ImVec4(0.84f, 0.65f, 0.26f, 1.0f), "Playlists");
        if (mediumFont) ImGui::PopFont();
        ImGui::Separator();
        
        if (showLoadedFiles) {
            ImGui::BeginChild("File List", ImVec2(0, 300), true);
            std::lock_guard<std::mutex> lock(filesMutex);
            for (const auto& file : loadedFiles) {
                if (ImGui::Selectable(file.c_str())) {
                    // Обработка выбора трека
                }
            }
            ImGui::EndChild();
        }
        
        if (ImGui::Button("Add Playlist", ImVec2(-1, 0))) {
            const char* folderPath = tinyfd_selectFolderDialog("Select Folder", nullptr);
            if (folderPath) {
                std::thread([folderPath, &loadedFiles, &showLoadedFiles, &filesMutex]() {
                    try {
                        std::vector<std::string> newFiles;
                        for (const auto& entry : fs::directory_iterator(folderPath)) {
                            if (entry.is_regular_file()) {
                                newFiles.push_back(entry.path().filename().string());
                            }
                        }
                        std::lock_guard<std::mutex> lock(filesMutex);
                        loadedFiles = std::move(newFiles);
                        showLoadedFiles = true;
                    } catch (...) {
                        std::cerr << "Error loading files" << std::endl;
                    }
                }).detach();
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
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImVec2 p1 = ImVec2(p0.x + cover_size, p0.y + cover_size);
            draw_list->AddRectFilled(p0, p1, IM_COL32(40, 40, 40, 255));
            
            // Информация о треке
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 200) * 0.6f);
            ImGui::SetCursorPosY(center_pos.y + cover_size + 30);
            if (mediumFont) ImGui::PushFont(mediumFont);
            ImGui::TextColored(ImVec4(0.84f, 0.65f, 0.26f, 1.0f), "Unknown Artist");
            if (mediumFont) ImGui::PopFont();
            
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 150) * 0.5f);
            ImGui::TextColored(ImVec4(0.61f, 0.56f, 0.41f, 1.0f), "No track selected");
            
            // Прогресс-бар трека
            ImGui::SetCursorPosX(30);
            ImGui::SetCursorPosY(center_pos.y + cover_size + 70);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60);
            ImGui::SliderFloat("##progress", &progress, 0.0f, 1.0f, "");
            
            // Время трека
            ImGui::SetCursorPosX(30);
            ImGui::TextColored(ImVec4(0.61f, 0.56f, 0.41f, 1.0f), "0:00");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 50);
            ImGui::TextColored(ImVec4(0.61f, 0.56f, 0.41f, 1.0f), "0:00");
        }
        ImGui::EndChild();

        // Панель управления
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
        ImGui::BeginChild("Controls", ImVec2(0, 105), true);
        {
            float buttonWidth = 50.0f;
            float playButtonWidth = 60.0f;
            float buttonHeight = 50.0f;
            float totalWidth = buttonWidth * 2 + playButtonWidth + ImGui::GetStyle().ItemSpacing.x * 2;
            float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
            ImGui::SetCursorPosX(startX);
            
            // Кнопки управления с PNG-иконками
            if (IconButton("prev", icons.prev, ImVec2(buttonWidth, buttonHeight))) {
                // Предыдущий трек
            }
            
            ImGui::SameLine();
            
            if (IconButton("play", isPlaying ? icons.pause : icons.play, ImVec2(playButtonWidth, playButtonWidth))) {
                isPlaying = !isPlaying;
            }
            
            ImGui::SameLine();
            
            if (IconButton("next", icons.next, ImVec2(buttonWidth, buttonHeight))) {
                // Следующий трек
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();

    ImGui::End();
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Настройка окна
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(900, 580, "CatMp3", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Установка GruvBox цветовой схемы
    SetupGruvboxStyle();

    // Инициализация бэкендов
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Загрузка ресурсов
    LoadResources();

    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Начало кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Основной интерфейс
        ShowMainInterface();

        // Рендеринг
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.14f, 0.13f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Очистка текстур
    glDeleteTextures(1, &icons.play);
    glDeleteTextures(1, &icons.pause);
    glDeleteTextures(1, &icons.stop);
    glDeleteTextures(1, &icons.prev);
    glDeleteTextures(1, &icons.next);

    // Очистка ImGui и GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}