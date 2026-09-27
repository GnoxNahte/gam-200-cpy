#pragma once

#include "../render/Model.h"
#include "../core/Platform.h"

class Fan : public Model
{
public:
    Fan();
    ~Fan() override;

    void InitModel()          override;
    void Render()             override;
    void Resize(int w, int h) override;


    bool TouchEventDown(float x, float y)    override;
    void TouchEventMove(float x, float y)    override;
    void TouchEventRelease(float x, float y) override;

private:
    bool checkBladeHit(float radius, float halfLength, float& outT);
    void RenderCube(const glm::vec3& color);

    GLuint program = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;

    // === Uniforms ===
    // Matrices
    GLint  uModelViewProjectionMatrix = -1;
    GLint  uModelViewMatrix = -1;
    GLint  uNormalMatrix = -1;

    // Material
    GLint  uMaterialAmbient     = -1;
    GLint  uMaterialSpecular    = -1;
    GLint  uMaterialDiffuse     = -1;

    // Light property
    GLint uLightAmbient     = -1;
    GLint uLightSpecular    = -1;
    GLint uLightDiffuse     = -1;

    GLint uLightPosition    = -1;
    GLint uShininessFactor  = -1;

    // VBO sub-region sizes
    GLsizeiptr posSize = 36 * 3 * sizeof(GLfloat);  // 96 bytes
    GLsizeiptr normalSize = 36 * 3 * sizeof(GLfloat);  // 96 bytes

    float spinAngle = 0.f;

    static constexpr int bladeCount = 20;

    bool selectedBlades[bladeCount] { };

    double prevTouchTime = -1.f;
    double touchTime = -1.f;
    glm::vec2 startTouchPos = { 0.f, 0.f };
    glm::vec2 prevTouchPos = { 0.f, 0.f };

    int windowWidth{ }, windowHeight{ };
    float debugf = 0.f;

    bool clicked = false;
    glm::vec3 rayOrigin{ };
    glm::vec3 rayDir{ };

    static constexpr float kBaseSpeed = 1.5f;
    static constexpr float kMaxBoost = 20.f;
    static constexpr float kBoostScale = 8.f;
    static constexpr float kTapThreshold = 12.f;
    
};
