cmake_minimum_required(VERSION 3.15)
project(MyProject)

# Настройка C++ стандарта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск GLFW и OpenGL
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Добавление исполняемого файла
add_executable(${PROJECT_NAME}
    main.cpp
    thirdparty/tinyfiledialogs/tinyfiledialogs.c
)

# Ручное подключение ImGui
file(GLOB IMGUI_SOURCES
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})

# Подключение заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyfiledialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

# Подключение библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::OpenGL
    glfw
)

# Добавляем определение для stb_image, чтобы включить реализацию функций
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STB_IMAGE_IMPLEMENTATION
)