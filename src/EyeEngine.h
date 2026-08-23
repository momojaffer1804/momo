// =====================================================
// EyeEngine.h — Expression Presets V2
// =====================================================

#ifndef EYE_ENGINE_H
#define EYE_ENGINE_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>


// =====================================================
// Eye State
// =====================================================

struct EyeState {

    // -------------------------------------------------
    // Position
    // -------------------------------------------------

    float x;
    float y;


    // -------------------------------------------------
    // Shape
    // -------------------------------------------------

    float width;
    float height;

    float rotation;
    float cornerRadius;


    // -------------------------------------------------
    // Eyelids
    //
    // Legacy values are retained so existing
    // AnimationEngine code continues to work.
    // -------------------------------------------------

    float upperLid;
    float lowerLid;


    // -------------------------------------------------
    // V2 Eyelid Geometry
    //
    // Positive values move the corresponding eyelid
    // toward the eye center.
    // -------------------------------------------------

    float upperLidInner;
    float upperLidOuter;

    float lowerLidInner;
    float lowerLidOuter;


    // -------------------------------------------------
    // Pupil
    // -------------------------------------------------

    float pupilX;
    float pupilY;
    float pupilSize;
};


// =====================================================
// Mouth
// =====================================================

enum MouthType {

    MOUTH_NONE,
    MOUTH_LINE,
    MOUTH_SMILE,
    MOUTH_SAD,
    MOUTH_OPEN
};


struct MouthState {

    MouthType type;

    float x;
    float y;

    float width;
    float height;

    float curvature;

    int thickness;
};


// =====================================================
// Eye Engine
// =====================================================

class EyeEngine {

public:

    EyeEngine(
        Adafruit_SSD1306* display
    );


    // -------------------------------------------------
    // Eye State
    // -------------------------------------------------

    void setEyes(
        const EyeState& left,
        const EyeState& right
    );

    void setLeftEye(
        const EyeState& eye
    );

    void setRightEye(
        const EyeState& eye
    );

    EyeState getLeftEye() const;

    EyeState getRightEye() const;


    // -------------------------------------------------
    // Mouth
    // -------------------------------------------------

    void setMouth(
        const MouthState& mouth
    );

    MouthState getMouth() const;


    // -------------------------------------------------
    // Rendering
    // -------------------------------------------------

    void draw();

    void drawEye(
        const EyeState& eye
    );


    // -------------------------------------------------
    // Reset
    // -------------------------------------------------

    void setNeutral();


private:

    Adafruit_SSD1306* _display;

    EyeState _leftEye;
    EyeState _rightEye;

    MouthState _mouth;


    // =================================================
    // Eye Rendering
    // =================================================

    void drawRotatedEye(
        const EyeState& eye
    );

    void drawPupil(
        const EyeState& eye
    );

    void drawEyelids(
        const EyeState& eye
    );


    // =================================================
    // Mouth Rendering
    // =================================================

    void drawMouth(
        const MouthState& mouth
    );
};

#endif