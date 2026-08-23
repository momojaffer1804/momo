#ifndef EXPRESSION_ENGINE_H
#define EXPRESSION_ENGINE_H

#include <Arduino.h>
#include "EyeEngine.h"


// =====================================================
// MoMo Expressions
// =====================================================

enum Expression {

    EXPRESSION_NEUTRAL,

    EXPRESSION_HAPPY,

    EXPRESSION_SAD,

    EXPRESSION_ANGRY,

    EXPRESSION_SURPRISED,

    EXPRESSION_SLEEPY
};


// =====================================================
// Expression Engine
// =====================================================

class ExpressionEngine {

public:

    ExpressionEngine(
        EyeEngine* eyeEngine
    );


    // -------------------------------------------------
    // Apply an expression immediately
    // -------------------------------------------------

    void setExpression(
        Expression expression
    );


    // -------------------------------------------------
    // Get currently active expression
    // -------------------------------------------------

    Expression getExpression() const;


    // -------------------------------------------------
    // Generate eye states for an expression
    // -------------------------------------------------

    void getExpressionStates(
        Expression expression,
        EyeState& left,
        EyeState& right
    );


    // -------------------------------------------------
    // Generate mouth state for an expression
    // -------------------------------------------------

    MouthState getExpressionMouth(
        Expression expression
    );


private:

    EyeEngine* _eyeEngine;

    Expression _currentExpression;


    // =================================================
    // Eye Expression Builders
    // =================================================

    void createNeutral(
        EyeState& left,
        EyeState& right
    );

    void createHappy(
        EyeState& left,
        EyeState& right
    );

    void createSad(
        EyeState& left,
        EyeState& right
    );

    void createAngry(
        EyeState& left,
        EyeState& right
    );

    void createSurprised(
        EyeState& left,
        EyeState& right
    );

    void createSleepy(
        EyeState& left,
        EyeState& right
    );
};

#endif