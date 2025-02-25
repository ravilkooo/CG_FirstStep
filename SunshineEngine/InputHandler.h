#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <unordered_map>
#include <Windows.h>
#include <iostream>

class InputHandler
{
    friend class DisplayWindow;
public:
    InputHandler();

    void Update();
    void UpdateKeyState(UINT vKey, bool isPressed);
    bool IsKeyDown(UINT vKey);

private:
    bool keys[256];
};

#endif // INPUTHANDLER_H