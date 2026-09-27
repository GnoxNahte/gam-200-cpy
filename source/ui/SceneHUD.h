#pragma once

#include "../render/Model.h"
#include <glm/fwd.hpp>
#include <functional>
#include <vector>

class SceneHUD : public Model
{
public:
	SceneHUD();
	~SceneHUD() override;
	void InitModel() override;
	void Render() override;
	void Resize(int w, int h) override;

	bool TouchEventDown(float x, float y) override;
	void TouchEventRelease(float x, float y) override;

	float CurrentSpeed() const { return speed; }

	// Singleton
	static SceneHUD* Instance;

private:
	struct Button
	{
		glm::vec3 color{};
		glm::vec2 pos{};
		glm::vec2 size{};

		bool isClicking{ false };
		std::function<void()> onClick;
	};

	static constexpr int MaxSpeed = 20;
	static constexpr int camDistStepMin = 3;
	static constexpr int camDistStepMax = 20;

	int targetSpeed = 8;
	float speed = 8.f;
	float acceleration = 0.f;
	int camDistStep = 0;

	int windowWidth{}, windowHeight{};

	static constexpr float maxRepeatTime = 0.4f;
	static constexpr float minRepeatTime = 0.075f;
	static constexpr float timeToMinRepeat = 1.f;

	float holdRepeatTime = maxRepeatTime;
	float firstTouchedTime = -1.f;
	float lastTouchedTime = -1.f;

	GLuint programID = 0;
	GLuint vao = 0;
	GLuint vboPos = 0;
	GLuint vboColor = 0;
	
	GLint uModelViewProjectionMatrix = -1;
	GLint uMaterialColor = -1;

	std::vector<Button> buttons;

	bool ProcessClickButton(Button& button, glm::vec2 clickPos, bool isReleasing);

	void RenderButton(const Button& button);
	void RenderSquare(const glm::vec3& color, const glm::vec2& pos, const glm::vec2& size);
};
