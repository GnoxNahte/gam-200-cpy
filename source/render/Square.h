#pragma once

/**
 * Square.h
 *
 * Self-contained rotating Square renderer (shader, VAO, VBOs).
 * Used by Desktop (Windows/GLFW) and Emscripten builds.
 * Android uses NativeTemplate.cpp (monolithic JNI) instead.
 */

#include "../core/Platform.h"
#include "Model.h"

class Square : public Model {
public:
    Square() = default;
    ~Square() override;

    void InitModel() override;
    void Render() override;
    void Resize(int w, int h) override;

private:

    GLuint programID    = 1;
    GLuint vao          = 1;
    GLuint vboPos       = 1;
    GLuint vboColor     = 1;
    GLint  uRadianAngle = -1;
    float  degree       = 0.0f;
};
