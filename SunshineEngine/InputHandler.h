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
        UP = 38u, DOWN = 40u
    };

    InputHandler();

    void Update();
    void UpdateKeyState(UINT vKey, bool isPressed);
    bool IsKeyDown(KeyCode vKey);
        
private:
    bool keys[256];
};

#endif // INPUTHANDLER_H