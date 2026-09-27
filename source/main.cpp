/**
 * main.cpp
 *
 * Entry point for Desktop (Windows/GLFW) and WebGL (Emscripten) builds.
 * Android uses NativeTemplate.cpp + JNI instead of this file.
 *
 * Emscripten supports two windowing backends (selected at compile time):
 *   USE_GLFW defined  → GLFW via -s USE_GLFW=3  (default, recommended)
 *   USE_GLFW absent   → SDL2 via -s USE_SDL=2
 *
 * The triangle rotates automatically – no user interaction required.
 * This is the introductory GLPI Framework demo.
 *
 * Ported from OpenGL ES 3.0 Cookbook – Chapter 2, GLPI Framework Intro.
 */

#include "core/Platform.h"
#include "render/Renderer.h"

// ==========================================================================
// WebGL / Emscripten
// ==========================================================================
#ifdef PLATFORM_EMSCRIPTEN

// --------------------------------------------------------------------------
// GLFW backend (default – build with -DUSE_GLFW -s USE_GLFW=3)
// --------------------------------------------------------------------------
#ifdef USE_GLFW

static GLFWwindow* g_window = nullptr;
static bool        g_mouseDown = false;

static void fbsize_cb(GLFWwindow* /*win*/, int w, int h)
{
    Renderer::Instance().Resize(w, h);
}

static void mouseButtonCB(GLFWwindow* win, int btn, int action, int)
{
    if (btn != GLFW_MOUSE_BUTTON_LEFT) return;
    double mx, my; glfwGetCursorPos(win, &mx, &my);
    if (action == GLFW_PRESS)
    {
        g_mouseDown = true;
        Renderer::Instance().TouchEventDown((float)mx, (float)my);
    }
    else if (action == GLFW_RELEASE)
    {
        g_mouseDown = false;
        Renderer::Instance().TouchEventRelease((float)mx, (float)my);
    }
}

static void main_loop()
{
    if (glfwWindowShouldClose(g_window)) {
        emscripten_cancel_main_loop();
        glfwDestroyWindow(g_window);
        glfwTerminate();
        return;
    }
    glfwPollEvents();
    if (g_mouseDown)
    {
        double mx, my; glfwGetCursorPos(g_window, &mx, &my);
        Renderer::Instance().TouchEventMove((float)mx, (float)my);
    }
    Renderer::Instance().Render();
    glfwSwapBuffers(g_window);
}

int main()
{
    LOGI("GLPI Framework Intro - WebGL / Emscripten (GLFW)");

    if (!glfwInit()) { LOGE("glfwInit failed"); return -1; }

    glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    g_window = glfwCreateWindow(800, 600, "Programming Quiz 3 - Fan Dashboard HUD", nullptr, nullptr);
    if (!g_window) { LOGE("glfwCreateWindow failed"); glfwTerminate(); return -1; }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, fbsize_cb);
    glfwSetMouseButtonCallback(g_window,     mouseButtonCB);

    Renderer::Instance().InitializeRenderer();
    int w, h;
    glfwGetFramebufferSize(g_window, &w, &h);
    Renderer::Instance().Resize(w, h);

    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}

// --------------------------------------------------------------------------
// SDL2 backend (opt-in – build without -DUSE_GLFW, use -s USE_SDL=2)
// --------------------------------------------------------------------------
#else // !USE_GLFW

static SDL_Window*   g_window = nullptr;
static SDL_GLContext g_glctx  = nullptr;

static void main_loop()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            emscripten_cancel_main_loop(); break;
        case SDL_MOUSEBUTTONDOWN:
            Renderer::Instance().TouchEventDown(
                (float)ev.button.x, (float)ev.button.y); break;
        case SDL_MOUSEMOTION:
            Renderer::Instance().TouchEventMove(
                (float)ev.button.x, (float)ev.button.y); break;
        case SDL_MOUSEBUTTONUP:
            Renderer::Instance().TouchEventRelease(
                (float)ev.button.x, (float)ev.button.y); break;
        default: break;
        }
    }
    Renderer::Instance().Render();
    SDL_GL_SwapWindow(g_window);
}

int main()
{
    LOGI("GLPI Framework Intro : WebGL / Emscripten (SDL2)");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    g_window = SDL_CreateWindow(
        "Programming Quiz 3 - Fan Dashboard HUD",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!g_window) { LOGE("SDL_CreateWindow failed: %s", SDL_GetError()); return -1; }

    g_glctx = SDL_GL_CreateContext(g_window);
    if (!g_glctx) { LOGE("SDL_GL_CreateContext failed: %s", SDL_GetError()); return -1; }

    Renderer::Instance().InitModel();
    Renderer::Instance().Resize(800, 600);

    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}

#endif // USE_GLFW

// ==========================================================================
// Desktop – Windows / GLFW
// ==========================================================================
#elif defined(PLATFORM_WINDOWS)

#include <iostream>

static bool g_mouseDown = false;

static void framebufferSizeCB(GLFWwindow* /*win*/, int w, int h)
{
    Renderer::Instance().Resize(w, h);
}

static void mouseButtonCB(GLFWwindow* win, int btn, int action, int)
{
    if (btn != GLFW_MOUSE_BUTTON_LEFT) return;
    double mx, my; glfwGetCursorPos(win, &mx, &my);
    if (action == GLFW_PRESS)
    {
        g_mouseDown = true;
        Renderer::Instance().TouchEventDown((float)mx, (float)my);
    }
    else if (action == GLFW_RELEASE)
    {
        g_mouseDown = false;
        Renderer::Instance().TouchEventRelease((float)mx, (float)my);
    }
}

int main()
{
    std::cout << "SPACE - Desktop (GLFW)\n";

    if (!glfwInit()) { std::cerr << "glfwInit failed\n"; return -1; }

    glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_ANY_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "SPACE", nullptr, nullptr);
    if (!window) {
        std::cerr << "glfwCreateWindow failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "glewInit failed\n"; return -1; }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCB);
    glfwSetMouseButtonCallback(window, mouseButtonCB);

    Renderer::Instance().InitializeRenderer();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    Renderer::Instance().Resize(w, h);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (g_mouseDown)
        {
            double mx, my; glfwGetCursorPos(window, &mx, &my);
            Renderer::Instance().TouchEventMove((float)mx, (float)my);
        }

        Renderer::Instance().Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // PLATFORM_WINDOWS
