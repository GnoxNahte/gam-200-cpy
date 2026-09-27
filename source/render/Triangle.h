#pragma once

/**
 * Triangle.h
 *
 * Self-contained rotating triangle renderer (shader, VAO, VBOs).
 * Used by Desktop (Windows/GLFW) and Emscripten builds.
 * Android uses NativeTemplate.cpp (monolithic JNI) instead.
 */

#include "../core/Platform.h"
#include "Model.h"

class Triangle : public Model {
public:
    Triangle() = default;
    ~Triangle() override;

    void InitModel() override;
    void Render() override;
    void Resize(int w, int h) override;

private:

    GLuint programID    = 0;
    GLuint vao          = 0;
    GLuint vboPos       = 0;
    GLuint vboColor     = 0;
    GLint  uRadianAngle = -1;
    float  degree       = 0.0f;
};
