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
        LEFT = 37u, UP = 38u, RIGHT = 39u, DOWN = 40u
    };

    InputHandler();

    void Update();
    void UpdateKeyState(UINT vKey, bool isPressed);
    bool IsKeyDown(KeyCode vKey);
        
private:
    bool keys[256];
};

#endif // INPUTHANDLER_H