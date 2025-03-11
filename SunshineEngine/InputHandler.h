#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <unordered_map>
#include <Windows.h>
#include <iostream>

class InputHandler
{

    friend class DisplayWindow;
public:
    enum class KeyCode : UINT {
        SPACE = 0x20u,
        D_1 = 0x31u,
        D_2 = 0x32u,
        D_3 = 0x33u,
        D_4 = 0x34u,
        D_5 = 0x35u,
        D_6 = 0x36u,

        LEFT = 0x25u,
        UP = 0x26u,
        RIGHT = 0x27u,
        DOWN = 0x28u,

        A = 0x41u,
        B = 0x42u,
        D = 0x44u,
        E = 0x45u,
        Q = 0x51u,
        R = 0x52u,
        S = 0x53u,
        W = 0x57u,
    };

    InputHandler();

    void Update();
    void UpdateKeyState(UINT vKey, bool isPressed);
    bool IsKeyDown(KeyCode vKey);
        
private:
    bool keys[256];
};

#endif // INPUTHANDLER_H