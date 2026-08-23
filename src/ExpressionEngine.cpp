#include "ExpressionEngine.h"


// =====================================================
// Constructor
// =====================================================

ExpressionEngine::ExpressionEngine(
    EyeEngine* eyeEngine
)
    : _eyeEngine(eyeEngine),
      _currentExpression(EXPRESSION_NEUTRAL) {
}


// =====================================================
// Set Expression
// =====================================================

void ExpressionEngine::setExpression(
    Expression expression
) {

    if (_eyeEngine == nullptr) {
        return;
    }

    EyeState left;
    EyeState right;

    getExpressionStates(
        expression,
        left,
        right
    );

    _eyeEngine->setEyes(
        left,
        right
    );

    _eyeEngine->setMouth(
        getExpressionMouth(expression)
    );

    _currentExpression = expression;
}


// =====================================================
// Get Current Expression
// =====================================================

Expression ExpressionEngine::getExpression() const {

    return _currentExpression;
}


// =====================================================
// Get Expression Eye States
// =====================================================

void ExpressionEngine::getExpressionStates(
    Expression expression,
    EyeState& left,
    EyeState& right
) {

    switch (expression) {

        case EXPRESSION_HAPPY:
            createHappy(left, right);
            break;

        case EXPRESSION_SAD:
            createSad(left, right);
            break;

        case EXPRESSION_ANGRY:
            createAngry(left, right);
            break;

        case EXPRESSION_SURPRISED:
            createSurprised(left, right);
            break;

        case EXPRESSION_SLEEPY:
            createSleepy(left, right);
            break;

        case EXPRESSION_NEUTRAL:
        default:
            createNeutral(left, right);
            break;
    }
}


// =====================================================
// Get Expression Mouth
// =====================================================

MouthState ExpressionEngine::getExpressionMouth(
    Expression expression
) {

    MouthState mouth;

    // Default = neutral

    mouth.type = MOUTH_LINE;
    mouth.x = 64;
    mouth.y = 55;
    mouth.width = 18;
    mouth.height = 2;
    mouth.curvature = 0;
    mouth.thickness = 2;


    switch (expression) {

        // -------------------------------------------------
        // HAPPY
        // -------------------------------------------------

        case EXPRESSION_HAPPY:

            mouth.type = MOUTH_SMILE;
            mouth.x = 64;
            mouth.y = 53;
            mouth.width = 24;
            mouth.height = 7;
            mouth.curvature = 1;
            mouth.thickness = 2;

            break;


        // -------------------------------------------------
        // SAD
        // -------------------------------------------------

        case EXPRESSION_SAD:

            mouth.type = MOUTH_SAD;
            mouth.x = 64;
            mouth.y = 57;
            mouth.width = 22;
            mouth.height = 7;
            mouth.curvature = -1;
            mouth.thickness = 2;

            break;


        // -------------------------------------------------
        // ANGRY
        // -------------------------------------------------

        case EXPRESSION_ANGRY:

            mouth.type = MOUTH_LINE;
            mouth.x = 64;
            mouth.y = 56;
            mouth.width = 18;
            mouth.height = 1;
            mouth.curvature = -1;
            mouth.thickness = 1;

            break;


        // -------------------------------------------------
        // SURPRISED
        // -------------------------------------------------

        case EXPRESSION_SURPRISED:

            mouth.type = MOUTH_OPEN;
            mouth.x = 64;
            mouth.y = 54;
            mouth.width = 11;
            mouth.height = 9;
            mouth.curvature = 0;
            mouth.thickness = 2;

            break;


        // -------------------------------------------------
        // SLEEPY
        // -------------------------------------------------

        case EXPRESSION_SLEEPY:

            mouth.type = MOUTH_LINE;
            mouth.x = 64;
            mouth.y = 56;
            mouth.width = 14;
            mouth.height = 1;
            mouth.curvature = 0;
            mouth.thickness = 1;

            break;


        // -------------------------------------------------
        // NEUTRAL
        // -------------------------------------------------

        case EXPRESSION_NEUTRAL:
        default:

            break;
    }

    return mouth;
}


// =====================================================
// NEUTRAL
// Clean baseline
// =====================================================

void ExpressionEngine::createNeutral(
    EyeState& left,
    EyeState& right
) {

    left.x = 39;
    left.y = 33;

    left.width = 38;
    left.height = 30;

    left.rotation = 0;
    left.cornerRadius = 8;

    // Legacy eyelids

    left.upperLid = 0;
    left.lowerLid = 0;

    // V2 eyelids

    left.upperLidInner = 0;
    left.upperLidOuter = 0;

    left.lowerLidInner = 0;
    left.lowerLidOuter = 0;

    // Pupil

    left.pupilX = 0;
    left.pupilY = 0;
    left.pupilSize = 7;


    right = left;

    right.x = 89;
}


// =====================================================
// HAPPY
// More relaxed / curved eyes
// =====================================================

void ExpressionEngine::createHappy(
    EyeState& left,
    EyeState& right
) {

    createNeutral(
        left,
        right
    );


    left.height = 22;
    right.height = 22;

    left.y = 35;
    right.y = 35;


    left.cornerRadius = 10;
    right.cornerRadius = 10;


    // Relaxed lower eyelids

    left.lowerLid = 4;
    right.lowerLid = 4;

    left.lowerLidInner = 5;
    left.lowerLidOuter = 3;

    right.lowerLidInner = 5;
    right.lowerLidOuter = 3;


    // Slightly raised pupils

    left.pupilY = -1;
    right.pupilY = -1;
}


// =====================================================
// SAD
// Downward inner geometry
// =====================================================

void ExpressionEngine::createSad(
    EyeState& left,
    EyeState& right
) {

    createNeutral(
        left,
        right
    );


    left.y = 32;
    right.y = 32;


    // Subtle rotation

    left.rotation = -5;
    right.rotation = 5;


    // Upper eyelid geometry:
    //
    // Left eye:
    // inner side higher closure
    //
    // Right eye:
    // mirrored

    left.upperLid = 4;
    right.upperLid = 4;


    left.upperLidInner = 6;
    left.upperLidOuter = 2;

    right.upperLidInner = 6;
    right.upperLidOuter = 2;


    // Slight downward pupil position

    left.pupilY = 3;
    right.pupilY = 3;
}


// =====================================================
// ANGRY
// Strong slanted eyelids
// No black band dependency
// =====================================================

void ExpressionEngine::createAngry(
    EyeState& left,
    EyeState& right
) {

    createNeutral(
        left,
        right
    );


    left.y = 32;
    right.y = 32;


    // Strong inward-facing rotation

    left.rotation = 7;
    right.rotation = -7;


    // Slanted upper eyelids

    left.upperLid = 5;
    right.upperLid = 5;


    left.upperLidInner = 8;
    left.upperLidOuter = 2;

    right.upperLidInner = 8;
    right.upperLidOuter = 2;


    // Slightly lower pupils

    left.pupilY = 1;
    right.pupilY = 1;
}


// =====================================================
// SURPRISED
// Large balanced eyes
// =====================================================

void ExpressionEngine::createSurprised(
    EyeState& left,
    EyeState& right
) {

    createNeutral(
        left,
        right
    );


    left.width = 40;
    right.width = 40;

    left.height = 34;
    right.height = 34;


    left.y = 30;
    right.y = 30;


    left.cornerRadius = 12;
    right.cornerRadius = 12;


    // Completely open eyelids

    left.upperLid = 0;
    right.upperLid = 0;

    left.lowerLid = 0;
    right.lowerLid = 0;

    left.upperLidInner = 0;
    left.upperLidOuter = 0;

    right.upperLidInner = 0;
    right.upperLidOuter = 0;

    left.lowerLidInner = 0;
    left.lowerLidOuter = 0;

    right.lowerLidInner = 0;
    right.lowerLidOuter = 0;


    // Smaller pupils for wider-eye effect

    left.pupilSize = 5;
    right.pupilSize = 5;

    left.pupilY = -1;
    right.pupilY = -1;
}


// =====================================================
// SLEEPY
// Partially closed eyelids
// =====================================================

void ExpressionEngine::createSleepy(
    EyeState& left,
    EyeState& right
) {

    createNeutral(
        left,
        right
    );


    left.height = 16;
    right.height = 16;

    left.y = 35;
    right.y = 35;


    // Heavy upper eyelids

    left.upperLid = 5;
    right.upperLid = 5;


    left.upperLidInner = 6;
    left.upperLidOuter = 5;

    right.upperLidInner = 6;
    right.upperLidOuter = 5;


    // Slight lower eyelid closure

    left.lowerLidInner = 1;
    left.lowerLidOuter = 1;

    right.lowerLidInner = 1;
    right.lowerLidOuter = 1;


    // Pupils slightly lowered

    left.pupilY = 2;
    right.pupilY = 2;
}