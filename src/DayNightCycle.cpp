#include "DayNightCycle.h"

void DayNightCycle::update(float dt)
{
    float PI = 3.14159265358979323846;
    currentTime += dt;
    float time = fmod(currentTime / dayDuration, 1.f);
    float angle = time * 2.f * PI;
    
    position = glm::vec3(cos(angle), sin(angle), 0.f);
    direction = -glm::normalize(position);

    float height = glm::clamp(position.y, 0.f, 1.f);
    float dawnFactor = 1.f - fabs(height - 0.5f) * 2.f;
    dawnFactor = glm::clamp(dawnFactor, 0.f, 1.f);

    colour = glm::mix(nightColour, dawnColour, dawnFactor);
    colour = glm::mix(colour, dayColour, height);

    backgroundColour = glm::mix(nightColorBackground, sunriseColorBackgroiund, dawnFactor);
    backgroundColour = glm::mix(backgroundColour, dayColorBackground, height);

    float intensity = height;
    colour *= intensity;

    position *= 100.f;
}

glm::vec3 DayNightCycle::getColour()
{
    return colour;
}

glm::vec3 DayNightCycle::getPosition()
{
    return position;
}

glm::vec3 DayNightCycle::getDirection()
{
    return direction;
}

glm::vec3 DayNightCycle::getBackgroundColour()
{
    return backgroundColour;
}
