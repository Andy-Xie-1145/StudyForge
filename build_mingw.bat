@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   StudyForge Build Script
echo ========================================
echo.

set OUTDIR=build
set OBJDIR=build\obj
if not exist %OUTDIR% mkdir %OUTDIR%
if not exist %OBJDIR% mkdir %OBJDIR%

REM === Step 1: Compile .c files with gcc ===

set C_FLAGS=-O2 -D_GLFW_USE_CONFIG_H -Iglfw\include -Iglfw\src -Iglad\include

echo [1/2] Compiling GLFW and GLAD ...
for %%f in (
    glfw\src\init.c glfw\src\input.c glfw\src\monitor.c glfw\src\window.c
    glfw\src\context.c glfw\src\platform.c glfw\src\vulkan.c
    glfw\src\egl_context.c glfw\src\osmesa_context.c glfw\src\wgl_context.c
    glfw\src\win32_init.c glfw\src\win32_module.c glfw\src\win32_monitor.c
    glfw\src\win32_window.c glfw\src\win32_joystick.c glfw\src\win32_time.c
    glfw\src\win32_thread.c glfw\src\null_init.c glfw\src\null_monitor.c
    glfw\src\null_window.c glfw\src\null_joystick.c
    glad\src\glad.c
) do (
    for %%n in ("%%~nf") do set ONAME=%OBJDIR%\%%~nn.o
    gcc -c %C_FLAGS% "%%f" -o "!ONAME!"
    if !ERRORLEVEL! neq 0 (
        echo [ERROR] Failed: %%f
        pause
        exit /b 1
    )
)

REM === Step 2: Compile .cpp and link with g++ ===

echo [2/2] Compiling and linking ...
g++ -std=c++17 -O2 -Wall -Iglfw\include -Iglfw\src -Iimgui -Iimgui\backends -Iglad\include main.cpp imgui\imgui.cpp imgui\imgui_demo.cpp imgui\imgui_draw.cpp imgui\imgui_tables.cpp imgui\imgui_widgets.cpp imgui\backends\imgui_impl_glfw.cpp imgui\backends\imgui_impl_opengl3.cpp %OBJDIR%\*.o -lopengl32 -lgdi32 -luser32 -lshell32 -lole32 -o %OUTDIR%\studyforge.exe -static

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build succeeded!
echo   Output: %OUTDIR%\studyforge.exe
echo ========================================
echo.
pause
