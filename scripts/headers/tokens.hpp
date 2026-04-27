#ifndef TOKENS_H
#define TOKENS_H

enum class Token {
    F,
    G,
    f,
    Z,
    TurnLeft,
    TurnRight,
    TurnAround,
    PitchDown,
    PitchUp,
    RollLeft,
    RollRight,
    PushState,
    PopState,
    DecreaseWidth,
    IncreaseWidth,
    NextColor,
    DrawPolygon
};

#endif

// F: Move forward by distance d and draw a line.
// G: Move forward by distance d and draw a line (distinct variable for logic).
// f: Move forward by distance d without drawing a line.
// Z: Move forward along the Z-axis and draw a line.
// TurnLeft (+): Turn left (counter-clockwise) by angle delta.
// TurnRight (-): Turn right (clockwise) by angle delta.
// TurnAround (|): Rotate heading by 180 degrees.
// PitchDown (&): Pitch down around the local left axis.
// PitchUp (^): Pitch up around the local left axis.
// RollLeft (\): Roll left around the local heading axis.
// RollRight (/): Roll right around the local heading axis.
// PushState ([): Push current turtle position and orientation onto the stack.
// PopState (]): Pop turtle position and orientation from the stack.
// DecreaseWidth (!): Decrease the current line thickness.
// IncreaseWidth (#): Increase the current line thickness.
// NextColor ('): Increment the drawing color index.
// DrawPolygon (@): Draw a predefined polygon/sprite at the current position.