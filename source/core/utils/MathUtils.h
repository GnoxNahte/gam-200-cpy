#include <limits>

float SmoothDamp(float current, float target, float& currentSpeed, float smoothTime, float deltaTime,
                        float maxSpeed = std::numeric_limits<float>::infinity());
