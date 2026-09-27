#pragma once

/**
 * Platform.h
 *
 * Single-header platform detection for the GLPI Framework Intro demo.
 * Supports three targets:
 *   PLATFORM_EMSCRIPTEN  – WebGL 2.0 via Emscripten + SDL2
 *   PLATFORM_WINDOWS     – OpenGL (ES compat) via GLFW + GLEW
 *   PLATFORM_ANDROID     – OpenGL ES 3.0 via Android NDK
 *
 * IMPORTANT: Emscripten must be detected BEFORE Windows because emcc
 * defines both __EMSCRIPTEN__ AND _WIN32 on some toolchain setups.
 */

#if defined(__EMSCRIPTEN__) && !defined(PLATFORM_EMSCRIPTEN)
#    define PLATFORM_EMSCRIPTEN
#elif defined(_WIN32) && !defined(PLATFORM_WINDOWS)
#    define PLATFORM_WINDOWS
#elif defined(__ANDROID__) && !defined(PLATFORM_ANDROID)
#    define PLATFORM_ANDROID
#endif

#ifdef PLATFORM_EMSCRIPTEN
    #ifdef USE_GLFW
        // GLFW path – Emscripten provides GLFW when built with -s USE_GLFW=3.
        // GLFW_INCLUDE_ES3 tells glfw3.h to pull in <GLES3/gl3.h> automatically.
        #define GLFW_INCLUDE_ES3
        #include <GLFW/glfw3.h>
    #else
        // SDL2 path – built with -s USE_SDL=2
        #include <GLES3/gl3.h>
        #include <SDL2/SDL.h>
    #endif
    #include <emscripten.h>
    #include <cstdio>
    #include <cstring>

    #define LOGI(...) do { printf("[INFO] " __VA_ARGS__); printf("\n"); } while(0)
    #define LOGE(...) do { printf("[ERROR] " __VA_ARGS__); printf("\n"); } while(0)
    #define LOGD(...) do { printf("[DEBUG] " __VA_ARGS__); printf("\n"); } while(0)

#elif defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
    #include <cstdio>
    #include <cstring>

    #define LOGI(...) do { printf("[INFO] " __VA_ARGS__); printf("\n"); } while(0)
    #define LOGE(...) do { printf("[ERROR] " __VA_ARGS__); printf("\n"); } while(0)
    #define LOGD(...) do { printf("[DEBUG] " __VA_ARGS__); printf("\n"); } while(0)

#endif
