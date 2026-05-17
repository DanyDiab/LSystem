#pragma once

#include <string>
#include <unordered_map>

using namespace std;

enum class Token {
    F,
    G,
    f,
    g,
    Z,
    Yaw,
    TurnAround,
    Pitch,
    Roll,
    PushState,
    PopState,
    Width,
    NextColor,
    DrawPolygon,
    NonTerminal
};


// F: Move forward by distance d and draw a line.
// G: Move forward by distance d and draw a line (distinct variable for logic).
// f: Move forward by distance d without drawing a line.
// g: Move forward by distance d without drawing a line.
// Z: Move forward along the Z-axis and draw a line.
// Yaw (+/-): Rotate around the local up axis by angle delta (Positive = Left, Negative = Right).
// TurnAround (|): Rotate heading by 180 degrees.
// Pitch (&/^): Rotate around the local left axis by angle delta (Positive = Down, Negative = Up).
// Roll (\//): Rotate around the local heading axis by angle delta (Positive = Left, Negative = Right).
// PushState ([): Push current turtle position and orientation onto the stack.
// PopState (]): Pop turtle position and orientation from the stack.
// Width (!/#): Set the current line thickness to the specified parameter value.
// NextColor ('): Increment the drawing color index.
// DrawPolygon (@): Draw a predefined polygon/sprite at the current position.
// NonTerminal : Logic Variable not used for anything other than just control structure



inline Token charToToken(char charToConvert) {
    static const std::unordered_map<char, Token> tokenMap = {
        {'F', Token::F},
        {'G', Token::G},
        {'f', Token::f},
        {'g', Token::g},
        {'Z', Token::Z},
        {'+', Token::Yaw},
        {'-', Token::Yaw},
        {'|', Token::TurnAround},
        {'&', Token::Pitch},
        {'^', Token::Pitch},
        {'\\', Token::Roll},
        {'/', Token::Roll},
        {'[', Token::PushState},
        {']', Token::PopState},
        {'!', Token::Width},
        {'#', Token::Width},
        {'\'', Token::NextColor},
        {'@', Token::DrawPolygon}
    };

    std::unordered_map<char, Token>::const_iterator it = tokenMap.find(charToConvert);

    if (it == tokenMap.end()) {
        return Token::NonTerminal;
    }

    return it->second;
}



