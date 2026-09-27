#pragma once

#include "../core/Platform.h"
#include "Model.h"

class Ground : public Model
{
public:
    Ground();
    ~Ground() override;

    void TouchEventMove(float x, float y)    override;

    void InitModel() override;
    void Render() override;
    void Resize(int w, int h) override;

    float debugf = 0.f;
private:

    GLuint programID = 1;
    GLuint vao = 1;
    GLuint vboPos = 1;
    GLuint vboColor = 1;

    // === Uniforms ===
    // Matrices
    GLint  uModelViewProjectionMatrix = -1;
    GLint  uModelViewMatrix = -1;
    GLint  uNormalMatrix = -1;

    // Material
    GLint  uMaterialAmbient = -1;
    GLint  uMaterialSpecular = -1;
    GLint  uMaterialDiffuse = -1;

    // Light property
    GLint uLightAmbient = -1;
    GLint uLightSpecular = -1;
    GLint uLightDiffuse = -1;

    GLint uLightPosition = -1;
    GLint uShininessFactor = -1;
};
