cmake_minimum_required(VERSION 3.14)
project(StudyForge LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# ============================================================
# 查找系统依赖
# ============================================================
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ============================================================
# 通过 FetchContent 获取 Dear ImGui 源码
# ============================================================
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.7
)
FetchContent_Populate(imgui)  # 下载但不自动添加到构建

# ImGui 源文件列表
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# ============================================================
# GLAD 预生成文件（OpenGL 3.3 Core）
# ============================================================
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/glad")

# ============================================================
# 应用程序
# ============================================================
add_executable(studyforge
    main.cpp
    ${IMGUI_SOURCES}
    ${GLAD_DIR}/src/glad.c
)

target_include_directories(studyforge PRIVATE
    ${GLAD_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(studyforge PRIVATE
    glfw
    OpenGL::GL
)

# Windows 平台额外链接
if(WIN32)
    target_link_libraries(studyforge PRIVATE gdi32 user32 shell32 ole32 oleaut32)
endif()

# ============================================================
# 安装（可选）
# ============================================================
install(TARGETS studyforge RUNTIME DESTINATION bin)
