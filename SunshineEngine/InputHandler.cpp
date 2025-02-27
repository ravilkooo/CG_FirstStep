#include "InputHandler.h"

InputHandler::InputHandler()
{
    // Инициализация массива keys нулями
    for (size_t i = 0; i < 256; i++)
    {
        keys[i] = false;
    }
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
    //std::cout << "Key: " << vKey << " State: " << isPressed << "\n";
}

bool InputHandler::IsKeyDown(KeyCode vKey)
{
    return keys[static_cast<UINT>(vKey)];
}