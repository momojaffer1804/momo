#ifndef ANIMATION_ENGINE_H
#define ANIMATION_ENGINE_H

#include <Arduino.h>
#include "EyeEngine.h"


class AnimationEngine {

public:

    AnimationEngine(EyeEngine* eyeEngine);


    // =================================================
    // Animate Eyes + Mouth
    // =================================================

    void moveTo(
        const EyeState& left,
        const EyeState& right,
        const MouthState& mouth,
        unsigned long duration
    );


    // =================================================
    // Animate Eyes Only
    // =================================================

    void moveTo(
        const EyeState& left,
        const EyeState& right,
        unsigned long duration
    );


    // =================================================
    // Update Animation
    // =================================================

    void update();


    // =================================================
    // Animation Status
    // =================================================

    bool isAnimating() const;


private:

    EyeEngine* _eyeEngine;


    // Current animation starting states

    EyeState _startLeft;
    EyeState _startRight;

    MouthState _startMouth;


    // Target states

    EyeState _targetLeft;
    EyeState _targetRight;

    MouthState _targetMouth;


    // Timing

    unsigned long _startTime;
    unsigned long _duration;

    bool _animating;


    // =================================================
    // Interpolation
    // =================================================

    EyeState interpolateEye(
        const EyeState& start,
        const EyeState& target,
        float t
    );


    MouthState interpolateMouth(
        const MouthState& start,
        const MouthState& target,
        float t
    );


    // Smooth animation curve

    float easeInOut(
        float t
    );
};

#endif