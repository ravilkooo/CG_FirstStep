#ifndef GAME_H
#define GAME_H

#include "GameTimer.h"
#include "InputHandler.h"
#include "Scene.h"
#include "PhysicsEngine.h"
#include "RenderingSystem.h"
#include "DisplayWindow.h"


class Game
{
public:
    Game();
    virtual ~Game();

    virtual void Run();

    virtual void Update(float deltaTime) = 0;
    virtual void Render();


    GameTimer timer;
    Scene scene;
    PhysicsEngine* physEngine;
    RenderingSystem* renderer;

    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;

    float deltaTime = 0.0f;
};

#endif // GAME_H