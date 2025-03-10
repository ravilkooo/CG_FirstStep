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
        D_1 = 0x31u,
        D_2 = 0x32u,
        D_3 = 0x33u,
        D_4 = 0x34u,
        D_5 = 0x35u,
        D_6 = 0x36u,
        LEFT = 37u, UP = 38u, RIGHT = 39u, DOWN = 40u,
        E = 0x45u, Q = 0x51u,
    };

    InputHandler();

    void Update();
    void UpdateKeyState(UINT vKey, bool isPressed);
    bool IsKeyDown(KeyCode vKey);
        
private:
    bool keys[256];
};

#endif // INPUTHANDLER_H