#pragma once

#include "Model.h"
#include <vector>
#include <chrono>

class Renderer {
public:
    static Renderer& Instance()
    {
        static Renderer rd;
        return rd;
    }

    Renderer(const Renderer&) = delete;
    Renderer operator=(const Renderer&) = delete;

    bool InitializeRenderer();
    void Resize(int w, int h);
    void Render();

    void TouchEventDown(float x, float y);
    void TouchEventMove(float x, float y);
    void TouchEventRelease(float x, float y);

    double dt = 0.f;
    double time = 0.f;
    
    struct Camera
    {
        glm::vec3 center{ 0.f, 3.f, -8.f };
        glm::vec3 eye{ 10.f, 10.f, 10.f };
        glm::vec3 up{ 0.f, 1.f, 0.f };
    };

    Camera camera;

private:
    Renderer() = default;
    ~Renderer();

    void createModels();
    void initializeModels();
    void clearModels();

    std::chrono::high_resolution_clock::time_point prevTime;
    std::vector<Model*> models;
};

