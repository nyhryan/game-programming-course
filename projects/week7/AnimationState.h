#pragma once

#include <cstdint>

enum AnimationState : uint8_t
{
    ANIM_WALKING = 0,
    ANIM_WALKING_TO_GREETING,
    ANIM_GREETING,
    ANIM_GREETING_TO_WALKING
};

struct AnimationController
{
    const char *currentStateStr() const
    {
        switch (currentState)
        {
        case ANIM_WALKING:
            return "WALKING";
        case ANIM_WALKING_TO_GREETING:
            return "WALKING_TO_GREETING";
        case ANIM_GREETING:
            return "GREETING";
        case ANIM_GREETING_TO_WALKING:
            return "GREETING_TO_WALKING";
        default:
            return "UNKNOWN";
        }
    }

    AnimationState currentState = ANIM_WALKING;
    int walkingCycleCount = 0;
    int greetingCycleCount = 0;

    const int MAX_WALKING_CYCLES = 2;
    const int MAX_GREETING_CYCLES = 2;
};