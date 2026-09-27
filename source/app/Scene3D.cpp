#include "Scene3D.h"

Scene3D::Scene3D()
{
}

Scene3D::~Scene3D()
{
	if (fan)
		delete fan;
	
	if (ground)
		delete ground;
}

void Scene3D::InitModel()
{
	fan = new Fan();
	ground = new Ground();

	ground->InitModel();
	fan->InitModel();
}

void Scene3D::Render()
{
	if (fan)
		fan->Render();

	if (ground)
		ground->Render();
}

void Scene3D::Resize(int w, int h)
{
	if (fan)
		fan->Resize(w, h);

	if (ground)
		ground->Resize(w, h);
}

bool Scene3D::TouchEventDown(float x, float y)
{
	fan->TouchEventDown(x, y);
	ground->TouchEventDown(x, y);

	return false;
}

void Scene3D::TouchEventMove(float x, float y)
{
	fan->TouchEventMove(x, y);
	ground->TouchEventMove(x, y);
}

void Scene3D::TouchEventRelease(float x, float y)
{
	fan->TouchEventRelease(x, y);
	ground->TouchEventRelease(x, y);
}

void Scene3D::SetCameraDisatance(float d)
{
	(void)d;
}

bool Scene3D::PickAt(float screenX, float screenY)
{
    (void)screenX;
    (void)screenY;
	return false;
}
