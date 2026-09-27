#pragma once

#include "../core/Transform.h"

class Model {
public:
    Transform transform;

    Model() {}
    virtual ~Model() {}

    virtual void InitModel() = 0;
    virtual void Render() = 0;
    virtual void Render(glm::mat4) {};
    virtual void Resize(int , int ) {}

    virtual bool TouchEventDown(float , float ) { return false; }
    virtual void TouchEventMove(float , float ) {}
    virtual void TouchEventRelease(float , float ) {}
};
