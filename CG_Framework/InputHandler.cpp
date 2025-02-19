#include "InputHandler.h"

InputHandler::InputHandler()
{
    // ������������� ��������� ������
}

void InputHandler::Update()
{
    for (auto& key : keyStates)
    {
        key.second = (GetAsyncKeyState(key.first) & 0x8000) != 0;
    }
}

bool InputHandler::IsKeyDown(int key)
{
    return keyStates[key];
}