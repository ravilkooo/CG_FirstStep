#include "InputHandler.h"

InputHandler::InputHandler()
{
    // Инициализация массива keys нулями
    for (size_t i = 0; i < 256; i++)
    {
        keys[i] = KEY_STATE::NONE;
    }
}

void InputHandler::Update()
{
    for (size_t i = 0; i < 256; i++)
    {
        auto res = GetAsyncKeyState(i);
        if ((res & 0x8000) != 0)
            if (keys[i] == KEY_STATE::NONE)
                keys[i] = KEY_STATE::CLICKED;
            else
                keys[i] = KEY_STATE::PRESSED;
        else
            keys[i] = KEY_STATE::NONE;
    }
}

void InputHandler::UpdateKeyState(UINT vKey, bool keyState)
{
    if (keyState)
        if (keys[vKey] == KEY_STATE::NONE)
            keys[vKey] = KEY_STATE::CLICKED;
        else
            keys[vKey] = KEY_STATE::PRESSED;
    else
        keys[vKey] = KEY_STATE::NONE;
    //std::cout << "Key: " << vKey << " State: " << keyState << "\n";
}

bool InputHandler::IsKeyDown(KeyCode vKey)
{
    //return keys[static_cast<UINT>(vKey)];
    return (IsKeyPressed(vKey) || IsKeyClicked(vKey));
}

bool InputHandler::IsKeyPressed(KeyCode vKey)
{
    return (keys[static_cast<UINT>(vKey)] == KEY_STATE::PRESSED);
}

bool InputHandler::IsKeyClicked(KeyCode vKey)
{
    return (keys[static_cast<UINT>(vKey)] == KEY_STATE::CLICKED);
}
