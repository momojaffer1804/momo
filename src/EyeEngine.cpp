#include "EyeEngine.h"


// =====================================================
// Constructor
// =====================================================

EyeEngine::EyeEngine(
    Adafruit_SSD1306* display
)
    : _display(display) {

    setNeutral();
}


// =====================================================
// Set Both Eyes
// =====================================================

void EyeEngine::setEyes(
    const EyeState& left,
    const EyeState& right
) {

    _leftEye = left;
    _rightEye = right;
}


// =====================================================
// Set Left Eye
// =====================================================

void EyeEngine::setLeftEye(
    const EyeState& eye
) {

    _leftEye = eye;
}


// =====================================================
// Set Right Eye
// =====================================================

void EyeEngine::setRightEye(
    const EyeState& eye
) {

    _rightEye = eye;
}


// =====================================================
// Get Left Eye
// =====================================================

EyeState EyeEngine::getLeftEye() const {

    return _leftEye;
}


// =====================================================
// Get Right Eye
// =====================================================

EyeState EyeEngine::getRightEye() const {

    return _rightEye;
}


// =====================================================
// Set Mouth
// =====================================================

void EyeEngine::setMouth(
    const MouthState& mouth
) {

    _mouth = mouth;
}


// =====================================================
// Get Mouth
// =====================================================

MouthState EyeEngine::getMouth() const {

    return _mouth;
}


// =====================================================
// Neutral State
// =====================================================

void EyeEngine::setNeutral() {

    _leftEye.x = 39;
    _leftEye.y = 33;

    _leftEye.width = 38;
    _leftEye.height = 30;

    _leftEye.rotation = 0;
    _leftEye.cornerRadius = 8;

    _leftEye.upperLid = 0;
    _leftEye.lowerLid = 0;

    _leftEye.upperLidInner = 0;
    _leftEye.upperLidOuter = 0;

    _leftEye.lowerLidInner = 0;
    _leftEye.lowerLidOuter = 0;

    _leftEye.pupilX = 0;
    _leftEye.pupilY = 0;
    _leftEye.pupilSize = 7;


    _rightEye = _leftEye;

    _rightEye.x = 89;


    _mouth.type = MOUTH_LINE;

    _mouth.x = 64;
    _mouth.y = 55;

    _mouth.width = 18;
    _mouth.height = 2;

    _mouth.curvature = 0;
    _mouth.thickness = 2;
}


// =====================================================
// Draw Everything
// =====================================================

void EyeEngine::draw() {

    if (_display == nullptr) {
        return;
    }


    _display->clearDisplay();


    // Eyes

    drawEye(_leftEye);
    drawEye(_rightEye);


    // Mouth

    drawMouth(_mouth);


    _display->display();
}


// =====================================================
// Draw Single Eye
// =====================================================

void EyeEngine::drawEye(
    const EyeState& eye
) {

    drawRotatedEye(eye);
    drawEyelids(eye);
    drawPupil(eye);
}


// =====================================================
// Draw Rotated Eye
// =====================================================

void EyeEngine::drawRotatedEye(
    const EyeState& eye
) {

    if (_display == nullptr) {
        return;
    }


    float halfWidth =
        eye.width / 2.0f;

    float halfHeight =
        eye.height / 2.0f;


    // -------------------------------------------------
    // No rotation
    // -------------------------------------------------

    if (abs(eye.rotation) < 0.01f) {

        _display->fillRoundRect(
            (int)(eye.x - halfWidth),
            (int)(eye.y - halfHeight),
            (int)eye.width,
            (int)eye.height,
            (int)eye.cornerRadius,
            SSD1306_WHITE
        );

        return;
    }


    // -------------------------------------------------
    // Rotated rounded rectangle
    //
    // OLED is only 1-bit, so draw a filled polygon.
    // This gives a clean slanted-eye silhouette without
    // introducing a separate black band.
    // -------------------------------------------------

    float angle =
        eye.rotation *
        0.0174532925f;

    float c = cos(angle);
    float s = sin(angle);


    int x1 =
        (int)(
            eye.x +
            (-halfWidth * c) -
            (-halfHeight * s)
        );

    int y1 =
        (int)(
            eye.y +
            (-halfWidth * s) +
            (-halfHeight * c)
        );


    int x2 =
        (int)(
            eye.x +
            (halfWidth * c) -
            (-halfHeight * s)
        );

    int y2 =
        (int)(
            eye.y +
            (halfWidth * s) +
            (-halfHeight * c)
        );


    int x3 =
        (int)(
            eye.x +
            (halfWidth * c) -
            (halfHeight * s)
        );

    int y3 =
        (int)(
            eye.y +
            (halfWidth * s) +
            (halfHeight * c)
        );


    int x4 =
        (int)(
            eye.x +
            (-halfWidth * c) -
            (halfHeight * s)
        );

    int y4 =
        (int)(
            eye.y +
            (-halfWidth * s) +
            (halfHeight * c)
        );


    _display->fillTriangle(
        x1, y1,
        x2, y2,
        x3, y3,
        SSD1306_WHITE
    );


    _display->fillTriangle(
        x1, y1,
        x3, y3,
        x4, y4,
        SSD1306_WHITE
    );
}


// =====================================================
// Draw Pupil
// =====================================================

void EyeEngine::drawPupil(
    const EyeState& eye
) {

    if (_display == nullptr) {
        return;
    }


    if (eye.pupilSize <= 0) {
        return;
    }


    int px =
        (int)(
            eye.x +
            eye.pupilX
        );

    int py =
        (int)(
            eye.y +
            eye.pupilY
        );


    int radius =
        (int)(
            eye.pupilSize / 2.0f
        );


    if (radius <= 0) {
        return;
    }


    // OLED eyes are white.
    // Pupils are black holes.

    _display->fillCircle(
        px,
        py,
        radius,
        SSD1306_BLACK
    );
}


// =====================================================
// Draw V2 Eyelids
// =====================================================
//
// Instead of drawing a giant black rectangle across
// the eye, we erase only the required portions.
//
// Inner and outer values allow expressions to shape
// the eyelid differently on each side.
//
// This is what prevents the angry expression from
// producing the old unnatural black band.
// =====================================================

void EyeEngine::drawEyelids(
    const EyeState& eye
) {

    if (_display == nullptr) {
        return;
    }


    float upperInner =
        eye.upperLidInner;

    float upperOuter =
        eye.upperLidOuter;


    float lowerInner =
        eye.lowerLidInner;

    float lowerOuter =
        eye.lowerLidOuter;


    // -------------------------------------------------
    // Backward compatibility
    //
    // If V2 values are zero but legacy values exist,
    // use the legacy value.
    // -------------------------------------------------

    if (
        upperInner == 0 &&
        upperOuter == 0 &&
        eye.upperLid > 0
    ) {

        upperInner =
            eye.upperLid;

        upperOuter =
            eye.upperLid;
    }


    if (
        lowerInner == 0 &&
        lowerOuter == 0 &&
        eye.lowerLid > 0
    ) {

        lowerInner =
            eye.lowerLid;

        lowerOuter =
            eye.lowerLid;
    }


    // -------------------------------------------------
    // Nothing to draw
    // -------------------------------------------------

    if (
        upperInner <= 0 &&
        upperOuter <= 0 &&
        lowerInner <= 0 &&
        lowerOuter <= 0
    ) {

        return;
    }


    float left =
        eye.x -
        eye.width / 2.0f;


    float right =
        eye.x +
        eye.width / 2.0f;


    float top =
        eye.y -
        eye.height / 2.0f;


    float bottom =
        eye.y +
        eye.height / 2.0f;


    float center =
        eye.x;


    // =================================================
    // Upper eyelid
    // =================================================

    if (
        upperInner > 0 ||
        upperOuter > 0
    ) {

        // Left / outer side

        int outerHeight =
            max(
                0,
                (int)upperOuter
            );


        if (outerHeight > 0) {

            _display->fillTriangle(
                (int)left,
                (int)top,
                (int)center,
                (int)(top + upperInner),
                (int)left,
                (int)(top + outerHeight),
                SSD1306_BLACK
            );
        }


        // Right / inner side

        int innerHeight =
            max(
                0,
                (int)upperInner
            );


        if (innerHeight > 0) {

            _display->fillTriangle(
                (int)center,
                (int)top,
                (int)right,
                (int)top,
                (int)right,
                (int)(top + innerHeight),
                SSD1306_BLACK
            );
        }
    }


    // =================================================
    // Lower eyelid
    // =================================================

    if (
        lowerInner > 0 ||
        lowerOuter > 0
    ) {

        // Left / outer side

        int outerHeight =
            max(
                0,
                (int)lowerOuter
            );


        if (outerHeight > 0) {

            _display->fillTriangle(
                (int)left,
                (int)bottom,
                (int)center,
                (int)(bottom - lowerInner),
                (int)left,
                (int)(bottom - outerHeight),
                SSD1306_BLACK
            );
        }


        // Right / inner side

        int innerHeight =
            max(
                0,
                (int)lowerInner
            );


        if (innerHeight > 0) {

            _display->fillTriangle(
                (int)center,
                (int)bottom,
                (int)right,
                (int)bottom,
                (int)right,
                (int)(bottom - innerHeight),
                SSD1306_BLACK
            );
        }
    }
}


// =====================================================
// Draw Mouth
// =====================================================

void EyeEngine::drawMouth(
    const MouthState& mouth
) {

    if (_display == nullptr) {
        return;
    }


    if (mouth.type == MOUTH_NONE) {
        return;
    }


    int x =
        (int)mouth.x;

    int y =
        (int)mouth.y;

    int width =
        (int)mouth.width;

    int height =
        (int)mouth.height;


    int thickness =
        max(
            1,
            mouth.thickness
        );


    int left =
        x -
        width / 2;


    int top =
        y -
        height / 2;


    // =================================================
    // Straight Line
    // =================================================

    if (
        mouth.type ==
        MOUTH_LINE
    ) {

        _display->drawLine(
            left,
            y,
            left + width,
            y,
            SSD1306_WHITE
        );

        return;
    }


    // =================================================
    // Smile
    // =================================================

    if (
        mouth.type ==
        MOUTH_SMILE
    ) {

        for (
            int offset = 0;
            offset < thickness;
            offset++
        ) {

            for (
                int i = 0;
                i <= width;
                i++
            ) {

                float t =
                    (float)i /
                    (float)width;


                float curve =
                    sin(
                        t * PI
                    ) *
                    height;


                int px =
                    left +
                    i;


                int py =
                    y +
                    (int)curve;


                _display->drawPixel(
                    px,
                    py + offset,
                    SSD1306_WHITE
                );
            }
        }

        return;
    }


    // =================================================
    // Sad Mouth
    // =================================================

    if (
        mouth.type ==
        MOUTH_SAD
    ) {

        for (
            int offset = 0;
            offset < thickness;
            offset++
        ) {

            for (
                int i = 0;
                i <= width;
                i++
            ) {

                float t =
                    (float)i /
                    (float)width;


                float curve =
                    sin(
                        t * PI
                    ) *
                    height;


                int px =
                    left +
                    i;


                int py =
                    y -
                    (int)curve;


                _display->drawPixel(
                    px,
                    py - offset,
                    SSD1306_WHITE
                );
            }
        }

        return;
    }


    // =================================================
    // Open Mouth
    // =================================================

    if (
        mouth.type ==
        MOUTH_OPEN
    ) {

        _display->drawRoundRect(
            left,
            top,
            width,
            height,
            max(
                2,
                width / 3
            ),
            SSD1306_WHITE
        );

        return;
    }
}