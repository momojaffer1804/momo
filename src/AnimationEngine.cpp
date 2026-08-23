#include "AnimationEngine.h"


// =====================================================
// Constructor
// =====================================================

AnimationEngine::AnimationEngine(
    EyeEngine* eyeEngine
)
{
    _eyeEngine = eyeEngine;

    _startTime = 0;
    _duration = 0;

    _animating = false;
}


// =====================================================
// Ease In / Out
// =====================================================

float AnimationEngine::easeInOut(
    float t
)
{
    if (t < 0.0f)
        t = 0.0f;

    if (t > 1.0f)
        t = 1.0f;

    return t * t * (3.0f - 2.0f * t);
}


// =====================================================
// Interpolate Eye
// =====================================================

EyeState AnimationEngine::interpolateEye(
    const EyeState& start,
    const EyeState& target,
    float t
)
{
    EyeState result;


    // -------------------------------------------------
    // Position
    // -------------------------------------------------

    result.x =
        start.x +
        (target.x - start.x) * t;

    result.y =
        start.y +
        (target.y - start.y) * t;


    // -------------------------------------------------
    // Shape
    // -------------------------------------------------

    result.width =
        start.width +
        (target.width - start.width) * t;

    result.height =
        start.height +
        (target.height - start.height) * t;

    result.rotation =
        start.rotation +
        (target.rotation - start.rotation) * t;

    result.cornerRadius =
        start.cornerRadius +
        (target.cornerRadius - start.cornerRadius) * t;


    // -------------------------------------------------
    // Legacy Eyelids
    // -------------------------------------------------

    result.upperLid =
        start.upperLid +
        (target.upperLid - start.upperLid) * t;

    result.lowerLid =
        start.lowerLid +
        (target.lowerLid - start.lowerLid) * t;


    // -------------------------------------------------
    // V2 Eyelid Geometry
    // -------------------------------------------------

    result.upperLidInner =
        start.upperLidInner +
        (target.upperLidInner - start.upperLidInner) * t;

    result.upperLidOuter =
        start.upperLidOuter +
        (target.upperLidOuter - start.upperLidOuter) * t;

    result.lowerLidInner =
        start.lowerLidInner +
        (target.lowerLidInner - start.lowerLidInner) * t;

    result.lowerLidOuter =
        start.lowerLidOuter +
        (target.lowerLidOuter - start.lowerLidOuter) * t;


    // -------------------------------------------------
    // Pupil
    // -------------------------------------------------

    result.pupilX =
        start.pupilX +
        (target.pupilX - start.pupilX) * t;

    result.pupilY =
        start.pupilY +
        (target.pupilY - start.pupilY) * t;

    result.pupilSize =
        start.pupilSize +
        (target.pupilSize - start.pupilSize) * t;


    return result;
}


// =====================================================
// Interpolate Mouth
// =====================================================

MouthState AnimationEngine::interpolateMouth(
    const MouthState& start,
    const MouthState& target,
    float t
)
{
    MouthState result;


    // Mouth type is discrete.
    // Geometry itself is animated.

    result.type = target.type;


    result.x =
        start.x +
        (target.x - start.x) * t;

    result.y =
        start.y +
        (target.y - start.y) * t;

    result.width =
        start.width +
        (target.width - start.width) * t;

    result.height =
        start.height +
        (target.height - start.height) * t;

    result.curvature =
        start.curvature +
        (target.curvature - start.curvature) * t;


    // -------------------------------------------------
    // Thickness
    // -------------------------------------------------

    float thickness =
        start.thickness +
        (target.thickness - start.thickness) * t;

    result.thickness =
        (int)(thickness + 0.5f);


    return result;
}


// =====================================================
// Animate Eyes + Mouth
// =====================================================

void AnimationEngine::moveTo(
    const EyeState& left,
    const EyeState& right,
    const MouthState& mouth,
    unsigned long duration
)
{
    if (_eyeEngine == nullptr)
        return;


    // -------------------------------------------------
    // Current state becomes animation start
    // -------------------------------------------------

    _startLeft =
        _eyeEngine->getLeftEye();

    _startRight =
        _eyeEngine->getRightEye();

    _startMouth =
        _eyeEngine->getMouth();


    // -------------------------------------------------
    // Target state
    // -------------------------------------------------

    _targetLeft = left;
    _targetRight = right;
    _targetMouth = mouth;


    _startTime = millis();

    _duration =
        (duration == 0)
        ? 1
        : duration;

    _animating = true;
}


// =====================================================
// Animate Eyes Only
// =====================================================

void AnimationEngine::moveTo(
    const EyeState& left,
    const EyeState& right,
    unsigned long duration
)
{
    if (_eyeEngine == nullptr)
        return;


    MouthState currentMouth =
        _eyeEngine->getMouth();


    moveTo(
        left,
        right,
        currentMouth,
        duration
    );
}


// =====================================================
// Update
// =====================================================

void AnimationEngine::update()
{
    if (!_animating)
        return;


    if (_eyeEngine == nullptr)
    {
        _animating = false;
        return;
    }


    unsigned long now =
        millis();


    unsigned long elapsed =
        now - _startTime;


    float progress;


    if (elapsed >= _duration)
    {
        progress = 1.0f;
    }
    else
    {
        progress =
            (float)elapsed /
            (float)_duration;
    }


    // -------------------------------------------------
    // Smooth movement
    // -------------------------------------------------

    float t =
        easeInOut(progress);


    // -------------------------------------------------
    // Interpolate eyes
    // -------------------------------------------------

    EyeState currentLeft =
        interpolateEye(
            _startLeft,
            _targetLeft,
            t
        );

    EyeState currentRight =
        interpolateEye(
            _startRight,
            _targetRight,
            t
        );


    // -------------------------------------------------
    // Interpolate mouth
    // -------------------------------------------------

    MouthState currentMouth =
        interpolateMouth(
            _startMouth,
            _targetMouth,
            t
        );


    // -------------------------------------------------
    // Apply states
    // -------------------------------------------------

    _eyeEngine->setEyes(
        currentLeft,
        currentRight
    );

    _eyeEngine->setMouth(
        currentMouth
    );


    // -------------------------------------------------
    // Draw frame
    // -------------------------------------------------

    _eyeEngine->draw();


    // -------------------------------------------------
    // Animation finished
    // -------------------------------------------------

    if (progress >= 1.0f)
    {
        _animating = false;
    }
}


// =====================================================
// Is Animating?
// =====================================================

bool AnimationEngine::isAnimating() const
{
    return _animating;
}