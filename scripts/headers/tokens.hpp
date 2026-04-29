#ifndef TOKENS_H
#define TOKENS_H

#include <string>

using namespace std;

enum class Token {
    F,
    G,
    f,
    g,
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
    DrawPolygon,
    NonTerminal
};


// F: Move forward by distance d and draw a line.
// G: Move forward by distance d and draw a line (distinct variable for logic).
// f: Move forward by distance d without drawing a line.
// g: Move forward by distance d without drawing a line.
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
// NonTerminal : Logic Variable not used for anything other than just control structure



inline Token charToToken(char charToConvert){
    static const std::unordered_map<char, Token> tokenMap = {
        {'F', Token::F},
        {'G', Token::G},
        {'f', Token::f},
        {'g', Token::g},
        {'Z', Token::Z},
        {'+', Token::TurnLeft},
        {'-', Token::TurnRight},
        {'|', Token::TurnAround},
        {'&', Token::PitchDown},
        {'^', Token::PitchUp},
        {'\\', Token::RollLeft},
        {'/', Token::RollRight},
        {'[', Token::PushState},
        {']', Token::PopState},
        {'!', Token::DecreaseWidth},
        {'#', Token::IncreaseWidth},
        {'\'', Token::NextColor},
        {'@', Token::DrawPolygon}
    };

    std::unordered_map<char, Token>::const_iterator it = tokenMap.find(charToConvert);

    if(it == tokenMap.end()){
        return Token::NonTerminal;
    }

    return it->second;

}


#endif

