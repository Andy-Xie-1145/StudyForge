# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

The project requires no CMake. Use the two-step MinGW build:

```bash
# Step 1: Compile .c files (GLFW + GLAD) with gcc
gcc -c -O2 -D_GLFW_USE_CONFIG_H -Iglfw/include -Iglfw/src -Iglad/include \
    glfw/src/*.c glad/src/glad.c -o build/obj/<name>.o

# Step 2: Compile .cpp and link with g++
g++ -std=c++17 -O2 -Wall \
    -Iglfw/include -Iglfw/src -Iimgui -Iimgui/backends -Iglad/include \
    main.cpp imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp \
    imgui/imgui_tables.cpp imgui/imgui_widgets.cpp \
    imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp \
    build/obj/*.o -lopengl32 -lgdi32 -luser32 -lshell32 -lole32 \
    -o build/studyforge.exe -static
```

Or just run `build_mingw.bat`.

## Architecture

All application code is header-only inline functions, with a single `main.cpp`:

```
main.cpp  →  UI layer (4 ImGui tabs, GLFW/OpenGL init)
models.h  →  SimpleDate (Julian day arithmetic), Question, AppData structs
crud.h    →  add_question, edit_question, delete_question, filter_questions
sm2_engine.h → SM-2 spaced repetition (sm2_update, sm2_get_due_indices)
analytics.h  → TagStats, tag co-occurrence analysis (compute_tag_stats)
exam.h    →  generate_exam (urgency-sorted plain text paper)
persistence.h → load_data/save_data (field-marker .dat text format)
```

**Include order matters**: `glad/glad.h` must come before `GLFW/glfw3.h`, and `GLFW_INCLUDE_NONE` must be defined to prevent GLFW from pulling in the system's old `GL/gl.h`.

## Key Design Decisions

- **GLFW compiled from source** (not a pre-built library). The `glfw/src/glfw_config.h` was manually created to enable Win32+WGL platform. The full set of .c files needed: all `win32_*.c`, `null_*.c`, `wgl_context.c`, `egl_context.c`, `osmesa_context.c`, and the platform-neutral files (`init.c`, `input.c`, `monitor.c`, `window.c`, `context.c`, `platform.c`, `vulkan.c`).

- **GLAD is hand-written** (not generated from the glad web service). It covers only the OpenGL 3.3 Core functions actually used by ImGui's OpenGL3 backend. `glad.c` uses `GLAPI` (defined as `extern`) for variable definitions — this produces harmless gcc warnings about "initialized and declared extern".

- **Chinese text in sm2_engine.h** uses hex escape sequences (`"\xe5\xae\x8c\xe7\xbe\x8e"` for "完美") rather than raw UTF-8 literals, for compiler portability. The rest of the codebase uses raw UTF-8 in string literals.

- **ImGui v1.92.7**: Do NOT call `io.Fonts->Build()` — the new backend auto-builds font textures. Use `ImGuiChildFlags_Borders` (plural), not `ImGuiChildFlags_Border`.

## Data Format

The `.dat` file (`studyforge_data.dat`) is a human-readable text format. Multiline fields (stem, answer, error_reason) escape `\n` as literal `\\n` during save and reverse on load. Each question block is delimited by `!QUESTION` / `!END`. Fields are prefixed with `@field:value`.

## SM-2 Algorithm Summary

Quality 0-5. Q >= 3: success (interval 1→6→prev×EF). Q < 3: reset to rep=0, interval=1. EF formula: `EF + (0.1 - (5-Q)*(0.08 + (5-Q)*0.02))`, floor 1.3. New questions have next_review=today (immediately due).
