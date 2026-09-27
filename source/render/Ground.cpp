#define LOG_TAG "Ground"
#include "ShaderHelper.h"
#include "Ground.h"
#include <cmath>
#include <string>
#include "Renderer.h"

// ---------------------------------------------------------------------------
// Vertex data
// ---------------------------------------------------------------------------

static const GLfloat kPositions[][3] = {
    {-1.0f,  0.0f, -1.0f},
    { 1.0f,  0.0f, -1.0f},
    { 1.0f,  0.0f,  1.0f},
    {-1.0f,  0.0f,  1.0f},
};

// ---------------------------------------------------------------------------
// Ground
// ---------------------------------------------------------------------------

// Ground::Ground()  = default;

Ground::Ground()
{
    transform.TransformInit();
}

Ground::~Ground()
{
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vboPos);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

void Ground::InitModel()
{
    LOGI("Ground::InitModel");

    programID = ShaderHelper::buildProgramFromFile("GroundVertex.glsl", "GroundFragment.glsl");

    // programID = createProgram(kVertexShader, kFragmentShader);
    if (!programID) { LOGE("Ground: could not create program"); return; }

    // === Uniforms ===
    // Matrices
    uModelViewProjectionMatrix = glGetUniformLocation(programID, "ModelViewProjectionMatrix");
    uModelViewMatrix = glGetUniformLocation(programID, "ModelViewMatrix");
    uNormalMatrix = glGetUniformLocation(programID, "NormalMatrix");

    // Material
    uMaterialAmbient = glGetUniformLocation(programID, "MaterialAmbient");
    uMaterialSpecular = glGetUniformLocation(programID, "MaterialSpecular");
    uMaterialDiffuse = glGetUniformLocation(programID, "MaterialDiffuse");

    // Light
    uLightAmbient = glGetUniformLocation(programID, "LightAmbient");
    uLightSpecular = glGetUniformLocation(programID, "LightSpecular");
    uLightDiffuse = glGetUniformLocation(programID, "LightDiffuse");

    uLightPosition = glGetUniformLocation(programID, "LightPosition");
    uShininessFactor = glGetUniformLocation(programID, "ShininessFactor");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions), kPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    LOGI("Ground::InitModel done");
}

void Ground::Render()
{
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    //static float debugf = 0.f;
    //if (debugb)
    //    debugf -= 0.002f;
    //std::cout << debugf << "\n";

    if (!programID || !vao) return;

    glEnable(GL_DEPTH_TEST);
    glUseProgram(programID);

    transform.TransformSetMatrixMode(VIEW_MATRIX);
    transform.TransformLoadIdentity();
    Renderer::Camera& cam = Renderer::Instance().camera;
    transform.TransformLookAt(&cam.eye, &cam.center, &cam.up);

    transform.TransformSetMatrixMode(MODEL_MATRIX);
    transform.TransformLoadIdentity();

    // Ground transform
    constexpr float scale = 100.f;
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.f, -2.f, 0.f);
    transform.TransformScale(scale, scale, scale);

    glm::mat4* mvp = transform.TransformGetModelViewProjectionMatrix();
    glm::mat4* vp = transform.TransformGetModelViewMatrix();

    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(*vp)));

    glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, &(*mvp)[0][0]);
    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, &(*vp)[0][0]);
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, &normalMat[0][0]);

    float matClr = 0.25f;
    float matSpecular = 0.6f;
    glUniform3f(uMaterialAmbient, matClr, matClr, matClr);
    glUniform3f(uMaterialDiffuse, matClr, matClr, matClr);
    glUniform3f(uMaterialSpecular, matSpecular, matSpecular, matSpecular);
    glUniform1f(uShininessFactor, 40.f);
    // Light
    glUniform3f(uLightAmbient, 1.0f, 1.0f, 1.0f);
    glUniform3f(uLightDiffuse, 1.0f, 1.0f, 1.0f);
    glUniform3f(uLightSpecular, 1.0f, 1.0f, 1.0f);
    glUniform3f(uLightPosition, 0.0f, 10.0f, -25.f);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    transform.TransformPopMatrix();
}

void Ground::Resize(int w, int h)
{
    float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
    transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    transform.TransformSetPerspective(glm::radians(60.f), aspect, 0.01f, 1000.f, 0);
}

void Ground::TouchEventMove(float , float )
{
    //debugf = x;
}
