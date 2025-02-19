#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <unordered_map>
#include <Windows.h>

class InputHandler
{
public:
    InputHandler();

    void Update();
    bool IsKeyDown(int key);

private:
    std::unordered_map<int, bool> keyStates;
};

#endif // INPUTHANDLER_H