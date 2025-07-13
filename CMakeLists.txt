cmake_minimum_required(VERSION 3.10)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)

# Настройки для GLFW
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Добавление ImGui
include_directories(
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
    ${CMAKE_SOURCE_DIR}/thirdparty/tinyfiledialogs  # Добавлено
)

# Исходные файлы ImGui
file(GLOB IMGUI_SOURCES 
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

# Добавляем tinyfiledialogs как статическую библиотеку
add_library(tinyfiledialogs STATIC
    "thirdparty/tinyfiledialogs/tinyfiledialogs.c"
)

# Основной исполняемый файл
add_executable(app main.cpp ${IMGUI_SOURCES})

# Линковка библиотек
target_link_libraries(app
    glfw
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    tinyfiledialogs  # Добавлено
)

# Настройки компиляции
if(UNIX)
    target_compile_options(app PRIVATE -O2)
endif()