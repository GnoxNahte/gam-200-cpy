#include "MathUtils.h"
#include <algorithm>

// Reference:
// - https://github.com/Unity-Technologies/UnityCsReference/blob/59b03b8a0f179c0b7e038178c90b6c80b340aa9f/Runtime/Export/Math/Mathf.cs#L309
// - https://theswissbay.ch/pdf/Gentoomen%20Library/Game%20Development/Programming/Game%20Programming%20Gems%204.pdf#page=111
float SmoothDamp(float current, float target, float& currentSpeed, float smoothTime, float deltaTime,
                        float maxSpeed)
{
    float omega  = 2.f / smoothTime;
    float x      = omega * deltaTime;
    float exp    = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = std::clamp(current - target, -maxSpeed, maxSpeed);
    float tmp    = (currentSpeed + omega * change) * deltaTime;
    currentSpeed = (currentSpeed - omega * tmp) * exp;
    return target + (change + tmp) * exp;
}
