#define LOG_TAG "Fan"
#include "Fan.h"
#include "../render/ShaderHelper.h"
#include "../render/Renderer.h"
#include "../ui/SceneHUD.h"
#include <limits>
// --------
// Geometry
// --------

// Cube referenced from https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/2.2.basic_lighting_specular/basic_lighting_specular.cpp
static const GLfloat kPositions[][3] = {
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,

     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
};

static const GLfloat kNormals[][3] = {
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,

    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,

   -1.0f,  0.0f,  0.0f,
   -1.0f,  0.0f,  0.0f,
   -1.0f,  0.0f,  0.0f,
   -1.0f,  0.0f,  0.0f,
   -1.0f,  0.0f,  0.0f,
   -1.0f,  0.0f,  0.0f,

    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,

    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f
};

static constexpr GLuint ATTRIB_POSITION = 0;
static constexpr GLuint ATTRIB_NORMAL = 1;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

Fan::Fan()
{
    transform.TransformInit();

    //transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    //transform.TransformLoadIdentity();
    //transform.TransformSetPerspective(glm::radians(60.f), 1.f, 0.01f, 1000.f, 0);
}

Fan::~Fan() 
{
    if (vao) { glDeleteVertexArrays(1, &vao);  vao = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo);        vbo = 0; }
    if (ibo) { glDeleteBuffers(1, &ibo);        ibo = 0; }
    if (program) { glDeleteProgram(program);        program = 0; }
}

// ---------------------------------------------------------------------------
// InitModel – compile shaders, upload VBO/IBO, create VAO
// ---------------------------------------------------------------------------
void Fan::InitModel()
{
    LOGI("Fan::InitModel");

    program = ShaderHelper::buildProgramFromFile(
        "FanVertex.glsl",
        "FanFragment.glsl");

    //selectedBlades[0] = true;
    if (!program) { LOGE("Fan: failed to build shader program"); return; }
    // === Uniforms ===
    // Matrices
    uModelViewProjectionMatrix = glGetUniformLocation(program, "ModelViewProjectionMatrix");
    uModelViewMatrix = glGetUniformLocation(program, "ModelViewMatrix");
    uNormalMatrix = glGetUniformLocation(program, "NormalMatrix");

    // Material
    uMaterialAmbient = glGetUniformLocation(program, "MaterialAmbient");
    uMaterialSpecular = glGetUniformLocation(program, "MaterialSpecular");
    uMaterialDiffuse = glGetUniformLocation(program, "MaterialDiffuse");

    // Light
    uLightAmbient = glGetUniformLocation(program, "LightAmbient");
    uLightSpecular = glGetUniformLocation(program, "LightSpecular");
    uLightDiffuse = glGetUniformLocation(program, "LightDiffuse");

    uLightPosition = glGetUniformLocation(program, "LightPosition");
    uShininessFactor = glGetUniformLocation(program, "ShininessFactor");

    // VBO: positions sub-region then colours sub-region
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, posSize + normalSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, kPositions);
    glBufferSubData(GL_ARRAY_BUFFER, posSize, normalSize, kNormals);

    // VAO: captures attribute layout
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(ATTRIB_POSITION);
    glEnableVertexAttribArray(ATTRIB_NORMAL);
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)posSize);

    // Seal the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    LOGI("Fan::InitModel done (VAO=%u, VBO=%u, IBO=%u)", vao, vbo, ibo);
}

#pragma region Interaction

// ---------------------------------------------------------------------------
// Resize
// ---------------------------------------------------------------------------
void Fan::Resize(int w, int h)
{
    float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
    transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    transform.TransformSetPerspective(glm::radians(60.f), aspect, 0.01f, 1000.f, 0);
    windowWidth = w;
    windowHeight = h;
}

bool Fan::checkBladeHit(float radius, float halfLength, float& outT)
{
    const int SAMPLES = 9;
    float bestDist = FLT_MAX;
    float bestT = -1.0f;

    glm::mat4* modelMat = transform.TransformGetModelMatrix();
    for (int i = 0; i < SAMPLES; ++i)
    {
        float s = -halfLength + (2.0f * halfLength) * i / (SAMPLES - 1);
        glm::vec3 samplePoint = *modelMat * glm::vec4{ 0.0f, s, 0.0f, 1.0f };

        glm::vec3 toSample = samplePoint - rayOrigin;
        float t = glm::dot(toSample, rayDir);
        if (t < 0.0f) continue;                     // this sample is behind the camera

        glm::vec3 closestPoint = rayOrigin + rayDir * t;
        float dist = glm::length(samplePoint - closestPoint);
        if (dist < bestDist) { bestDist = dist; bestT = t; }
    }

    if (bestT < 0.0f || bestDist > radius) return false;
    outT = bestT;
    return true;
}

bool Fan::TouchEventDown(float x, float y)
{
    glm::mat4* viewMatrix = transform.TransformGetViewMatrix();
    glm::mat4* projMatrix = transform.TransformGetProjectionMatrix();
    int viewport[4] = { 0, 0, windowWidth, windowHeight };

    float nx, ny, nz, fx, fy, fz;
    transform.TransformUnproject(x, static_cast<float>(windowHeight) - y, 0.0f, viewMatrix, projMatrix, viewport, &nx, &ny, &nz);
    transform.TransformUnproject(x, static_cast<float>(windowHeight) - y, 1.0f, viewMatrix, projMatrix, viewport, &fx, &fy, &fz);

    rayOrigin = glm::vec3(nx, ny, nz);
    rayDir = glm::normalize(glm::vec3(fx, fy, fz) - rayOrigin);

    touchTime = Renderer::Instance().time;
    startTouchPos = prevTouchPos;

    clicked = true;

    return false;
}

void Fan::TouchEventMove(float , float )
{
    //debugf = x;
}

void Fan::TouchEventRelease(float , float )
{
}

#pragma endregion

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------
void Fan::Render()
{
    if (!program || !vao) return;

    // === Update ===

    spinAngle += SceneHUD::Instance->CurrentSpeed() * 10.f * static_cast<float>(Renderer::Instance().dt);
    if (spinAngle >= 360.f)
        spinAngle -= 360.f;

    // === Render ===

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

    glDisable(GL_CULL_FACE);

    transform.TransformSetMatrixMode(VIEW_MATRIX);
    transform.TransformLoadIdentity();
    Renderer::Camera& cam = Renderer::Instance().camera;
    transform.TransformLookAt(&cam.eye, &cam.center, &cam.up);

    transform.TransformSetMatrixMode(MODEL_MATRIX);
    transform.TransformLoadIdentity();

    constexpr float modelScale = 3.f;
    // World
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.f, 5.8f, -8.f);
    transform.TransformRotate(glm::radians(spinAngle), 0.f, 1.f, 0.f);
    transform.TransformScale(modelScale, modelScale, modelScale);

    // Base
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.f, -2.6f, 0.f);
    transform.TransformScale(1.6f, 0.25f, 0.8f);
    transform.TransformScale(0.5f, 0.5f, 0.5f);
    RenderCube(glm::vec3{0.45f, 0.28f, 0.12f});
    transform.TransformPopMatrix();

    // Pole
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.f, -1.21f, 0.f);
    transform.TransformScale(0.15f, 2.53f, 0.15f);
    transform.TransformScale(0.5f, 0.5f, 0.5f);
    RenderCube(glm::vec3{0.55f, 0.55f, 0.58f});
    transform.TransformPopMatrix();

    // Hub
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.f, 0.2f, 0.f);
    transform.TransformScale(0.3f, 0.3f, 0.3f);
    transform.TransformScale(0.5f, 0.5f, 1.f);
    RenderCube(glm::vec3{ 0.2f, 0.2f, 0.22f });
    transform.TransformPopMatrix();

    float bladeAngle = 360.f / bladeCount;
    //static constexpr std::array colors = {
    //    glm::vec3{1.0f, 0.0f, 0.0f},
    //    glm::vec3{0.0f, 1.0f, 0.0f},
    //    glm::vec3{0.0f, 0.0f, 1.0f},
    //    glm::vec3{1.0f, 0.6f, 0.0f},
    //};

    float bestT = std::numeric_limits<float>::max();
    int bestBladeIndex = -1;
    for (int i = 0; i < bladeCount; ++i)
    {
        constexpr float bladeSize = 0.08f;
        constexpr float bladeLength = 0.8f;
        // Blade
        transform.TransformPushMatrix();
        transform.TransformTranslate(0.f, 0.2f, 0.15f);
        transform.TransformRotate(glm::radians(spinAngle + i * bladeAngle), 0.f, 0.f, 1.f);
        transform.TransformTranslate(0.f, bladeLength + 0.15f, (i % 4) * 0.02f);
        transform.TransformScale(bladeSize, bladeLength, bladeSize);

        float t;
        if (clicked && checkBladeHit(bladeSize * modelScale * 1.5f, bladeLength * modelScale * 0.5f, t) && t < bestT)
        {
            bestT = t;
            bestBladeIndex = i;
        }

        constexpr glm::vec3 bladeColor{ 0.80f, 0.42f, 0.18f };
        //RenderCube(colors[i % colors.size()]);
        RenderCube(selectedBlades[i] ? glm::vec3{1.0f, 0.75f, 0.15f} : bladeColor);
        transform.TransformPopMatrix();
    }

    // Will only select on the next frame but should be ok
    if (bestBladeIndex >= 0)
    {
        selectedBlades[bestBladeIndex] = !selectedBlades[bestBladeIndex];
        //std::cout << "Selected: " << bestBladeIndex << "\n";
        LOGI("Scene3D: Blade %i picked - highlight %s", bestBladeIndex, selectedBlades[bestBladeIndex] ? "on" : "off");
    }

    transform.TransformPopMatrix();

    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);

    clicked = false;
}

void Fan::RenderCube(const glm::vec3& color)
{
    glm::mat4* mvp = transform.TransformGetModelViewProjectionMatrix();
    glm::mat4* vp = transform.TransformGetModelViewMatrix();
    
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(*vp)));

    glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, &(*mvp)[0][0]);
    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, &(*vp)[0][0]);
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, &normalMat[0][0]);

    glUniform3fv(uMaterialAmbient, 1, &color.x);
    glUniform3fv(uMaterialDiffuse, 1, &color.x);
    glUniform3f(uMaterialSpecular, 1.0f, 0.5f, 0.5f);
    glUniform1f(uShininessFactor, 40.0f);
    // Light
    float lightIntensity = 0.7f;
    glUniform3f(uLightAmbient,  lightIntensity, lightIntensity, lightIntensity);
    glUniform3f(uLightDiffuse,  lightIntensity, lightIntensity, lightIntensity);
    glUniform3f(uLightSpecular, lightIntensity, lightIntensity, lightIntensity);
    glUniform3f(uLightPosition, 0.0f, 0.0f, -25.f);
    //std::cout << debugf << "\n";
    // VAO encapsulates all attribute + IBO state: no per-draw setup required
    glBindVertexArray(vao);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
