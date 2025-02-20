#ifndef GAME_H
#define GAME_H

#include "GameTimer.h"
#include "InputHandler.h"
#include "Scene.h"
#include "PhysicsEngine.h"
#include "Renderer.h"
#include "DisplayWindow.h"

#include "Quad.h"
#include "Triangle.h"

class Game
{
public:
    Game();
    virtual ~Game();

    void Run();

protected:
    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void Render();

private:
    GameTimer timer;
    InputHandler inputHandler;
    Scene scene;
    PhysicsEngine physEngine;
    Renderer renderer;
    DisplayWindow displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;
};

#endif // GAME_H