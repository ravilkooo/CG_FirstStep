#include "InputHandler.h"

InputHandler::InputHandler()
{
    // Инициализация состояний клавиш
}

void InputHandler::Update()
{
    for (size_t i = 0; i < 256; i++)
    {
        keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

void InputHandler::UpdateKeyState(UINT vKey, bool isPressed)
{
    keys[vKey] = isPressed;
}

bool InputHandler::IsKeyDown(UINT vKey)
{
    return keys[vKey];
}
