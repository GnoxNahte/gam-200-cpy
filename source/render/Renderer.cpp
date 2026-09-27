#include "Renderer.h"
#include "../app/Scene3D.h"
#include "../ui/SceneHUD.h"

bool Renderer::InitializeRenderer() {
    createModels();
    initializeModels();

    prevTime = { std::chrono::high_resolution_clock::now() };
    return true;
}

void Renderer::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    for (auto i : models)
        i->Resize(w,h);
}

void Renderer::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto now{ std::chrono::high_resolution_clock::now() };
    dt = std::chrono::duration<double>(now - prevTime).count();
    time = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    prevTime = now;

    //for (auto i : models)
    //    i->Render();

    // Render Scene3D
    models[0]->Render();

    // Render SceneHUD
    glDisable(GL_DEPTH_TEST);
    models[1]->Render();

    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer()
{
    clearModels();
}

void Renderer::createModels()
{
    //models.push_back(new Triangle());
    //models.push_back(new Square());
    models.push_back(new Scene3D());
    models.push_back(new SceneHUD());
}

void Renderer::initializeModels()
{
    for (auto i : models)
        i->InitModel();
}

void Renderer::clearModels()
{
    for (auto i : models){
        delete i;
    }
    models.clear();
}

void Renderer::TouchEventDown(float x, float y)    { 
    for (Model* m : models)
    {
        bool ifClicked = m->TouchEventDown(x, y);
        if (ifClicked)
            return;
    }
}

void Renderer::TouchEventMove(float x, float y)    { for (Model* m : models) m->TouchEventMove(x,y); }
void Renderer::TouchEventRelease(float x, float y) { for (Model* m : models) m->TouchEventRelease(x,y); }
