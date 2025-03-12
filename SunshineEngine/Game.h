#ifndef GAME_H
#define GAME_H

#include "GameTimer.h"
#include "InputHandler.h"
#include "Scene.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "DisplayWindow.h"


class Game
{
public:
    Game();
    virtual ~Game();

    virtual void Run() = 0;

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

    GameTimer timer;
    //InputHandler inputHandler;
    Scene scene;
    PhysicsEngine* physEngine;
    Renderer renderer;
    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;
};

#endif // GAME_H