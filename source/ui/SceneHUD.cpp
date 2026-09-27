#include "SceneHUD.h"
#include "../render/ShaderHelper.h"
#include "../render/Renderer.h"
#include "../core/utils/MathUtils.h"

static const GLfloat kPositions[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

SceneHUD* SceneHUD::Instance = nullptr;

SceneHUD::SceneHUD()
{
    if (Instance)
    {
        LOGE("Scene HUD instance already exists!");
        return;
    }

    Instance = this;
    //targetSpeed = 0.f;
    camDistStep = 10;

    transform.TransformInit();

    // === Buttons ===
    glm::vec2 buttonSize{ 120.f, 50.f };
    // Speed up
    buttons.push_back(Button {
        glm::vec3{ 0.18, 0.69, 0.69 },
        glm::vec2{ 100.f, 70.f },
        buttonSize,
        false,
        [&]() { targetSpeed = std::min(targetSpeed + 1, MaxSpeed); LOGI("SceneHUD: +SPD pressed"); }
    });
    // Speed down
    buttons.push_back(Button {
        glm::vec3{ 0.18, 0.69, 0.69 },
        glm::vec2{ 100.f, 140.f },
        buttonSize,
        false,
        [&]() { targetSpeed = std::max(targetSpeed - 1, 0); LOGI("SceneHUD: -SPD pressed"); }
    });

    Renderer::Camera& cam = Renderer::Instance().camera;
    glm::vec3 viewDirection = glm::normalize(cam.center - cam.eye);

    // Zoom in
    buttons.push_back(Button {
        glm::vec3{ 0.47, 0.6, 0.86 },
        glm::vec2{ windowWidth - 100.f, 70.f },
        buttonSize,
        false,
        [&, viewDirection]() {
            if (camDistStep < camDistStepMax)
            {
                ++camDistStep;
                cam.eye += viewDirection;
                LOGI("SceneHUD: +ZOOM pressed");
            }
        }
    });
    // Zoom out
    buttons.push_back(Button {
        glm::vec3{ 0.47, 0.6, 0.86 },
        glm::vec2{ windowWidth - 100.f, 140.f },
        buttonSize,
        false,
        [&,viewDirection]() {
            if (camDistStep > camDistStepMin)
            {
                --camDistStep;
                cam.eye -= viewDirection;
                LOGI("SceneHUD: -ZOOM pressed");
            }
        }
    });
}

SceneHUD::~SceneHUD()
{
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vboPos);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

void SceneHUD::InitModel()
{
    LOGI("SceneHUD::InitModel");

    programID = ShaderHelper::buildProgramFromFile("UI_SquareVertex.glsl", "UI_SquareFrag.glsl");

    // programID = createProgram(kVertexShader, kFragmentShader);
    if (!programID) { LOGE("SceneHUD: could not create program"); return; }

    uModelViewProjectionMatrix = glGetUniformLocation(programID, "ModelViewProjectionMatrix");
    uMaterialColor = glGetUniformLocation(programID, "MaterialColor");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions), kPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    LOGI("SceneHUD::InitModel done");
}

void SceneHUD::Render()
{
    // === Update ===
    speed = SmoothDamp(speed, static_cast<float>(targetSpeed), acceleration, 0.8f, static_cast<float>(Renderer::Instance().dt));
    
    float t = static_cast<float>(Renderer::Instance().time);
    holdRepeatTime = glm::mix(maxRepeatTime, minRepeatTime, std::clamp((t - firstTouchedTime) / timeToMinRepeat, 0.f, 1.f));
    
    if (t - lastTouchedTime > holdRepeatTime)
    {
        for (auto& i : buttons)
        {
            if (i.isClicking)
            {
                i.onClick();
                lastTouchedTime = t;
                break;
            }
        }
    }

    // === Render ===
    glUseProgram(programID);

    transform.TransformSetMatrixMode(MODEL_MATRIX);
    transform.TransformLoadIdentity();

    for (auto& i : buttons)
        RenderButton(i);

    constexpr int barWidth = 10;
    constexpr int spacing = 2;
    constexpr glm::vec3 onClr{ 0.25, 0.85, 0.3 };
    constexpr glm::vec3 offClr{ 0.45, 0.45, 0.48 };
    for (int i = 0; i < MaxSpeed; ++i)
    {
        RenderSquare(
            i < targetSpeed ? onClr : offClr, 
            //{ windowWidth * 0.5f + i * (barWidth + spacing) - MaxSpeed / 2 * (barWidth + spacing), windowHeight - 50.f},
            { windowWidth * 0.5f + (i - MaxSpeed / 2) * (barWidth + spacing), windowHeight - 50.f},
            { barWidth , 20 }
        );
    }

    RenderSquare({ 0.31, 0.79, 0.69 },
        //{ windowWidth * 0.5f - MaxSpeed / 2 * (barWidth + spacing)  + speed * 0.5f * (barWidth + spacing) - (barWidth + spacing) * 0.5f, windowHeight - 30.f},
        { windowWidth * 0.5f + (-MaxSpeed / 2 + speed * 0.5f - 0.5f) * (barWidth + spacing), windowHeight - 30.f},
        { (barWidth + spacing) * speed - spacing, 20 }
    );

    for (int i = 0; i < camDistStepMax; ++i)
    {
        RenderSquare(
            i < camDistStep ? onClr : offClr,
            //{ windowWidth * 0.5f + i * (barWidth + spacing) - MaxSpeed / 2 * (barWidth + spacing), windowHeight - 50.f},
            { windowWidth - 50.f, windowHeight * 0.5f - (i - MaxSpeed / 2) * (barWidth + spacing) },
            { 20 , barWidth }
        );
    }
}

void SceneHUD::Resize(int w, int h)
{
    windowWidth = w; 
    windowHeight = h;

    buttons[2].pos.x = windowWidth - 100.f;
    buttons[3].pos.x = windowWidth - 100.f;

    transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    transform.TransformOrtho(0, static_cast<float>(w), static_cast<float>(h), 0, -1, 1);
}

bool SceneHUD::TouchEventDown(float x, float y)
{
    firstTouchedTime = lastTouchedTime = static_cast<float>(Renderer::Instance().time);
    for (auto& i : buttons)
    {
        bool ifClicked = ProcessClickButton(i, { x, y }, false);
        if (ifClicked)
            return true;
    }
	return false;
}

void SceneHUD::TouchEventRelease(float x, float y)
{
    for (auto& i : buttons)
    {
        ProcessClickButton(i, { x, y }, true);
        i.isClicking = false;
    }
    lastTouchedTime = std::numeric_limits<float>::lowest();
}

bool SceneHUD::ProcessClickButton(Button& button, glm::vec2 clickPos, bool isReleasing)
{
    glm::vec2 diff = glm::abs(button.pos - clickPos);
    bool clicked = diff.x < button.size.x * 0.5f && diff.y < button.size.y * 0.5f;
    if (clicked)
    {
        button.isClicking = !isReleasing;
        if (isReleasing)
            button.onClick();
    }

    return clicked;
}

void SceneHUD::RenderButton(const Button& button)
{
    if (button.isClicking)
        RenderSquare(button.color * 0.85f, button.pos, button.size);
    else
        RenderSquare(button.color, button.pos, button.size);
}

void SceneHUD::RenderSquare(const glm::vec3& color, const glm::vec2& pos, const glm::vec2& size)
{
    transform.TransformPushMatrix();
    transform.TransformTranslate(pos.x, pos.y, 0.f);
    transform.TransformScale(size.x, size.y, 1.f);
    
    glm::mat4* mvp = transform.TransformGetModelViewProjectionMatrix();

    glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, &(*mvp)[0][0]);
    
    //glm::mat4 tmp{ 1.f };
    //glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, &(tmp)[0][0]);
    
    glUniform3fv(uMaterialColor, 1, &color.x);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    transform.TransformPopMatrix();
}
