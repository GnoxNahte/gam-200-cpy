#pragma once

#include "../core/Platform.h"
#include "../render/Model.h"
#include "render/Ground.h"
#include "Fan.h"

class Scene3D : public Model
{
public:
	Scene3D();
	~Scene3D() override;

	void InitModel() override;
	void Render() override;
	void Resize(int w, int h) override;

	bool TouchEventDown(float x, float y)    override;
	void TouchEventMove(float x, float y)    override;
	void TouchEventRelease(float x, float y) override;

	void SetCameraDisatance(float d); // Called by SceneHUD's zoom buttons
	bool PickAt(float screenX, float screenY);

private:

	Fan* fan = nullptr;
	Ground* ground = nullptr;
	float cameraDistance = 8.f;
};
