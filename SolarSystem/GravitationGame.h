#pragma once
#include <Game.h>
#include "SolarSystemPhysics.h"
#include "GravitationBody.h"
#include <Camera.h>
#include <random>
#include "StarBox.h"

class GravitationGame :
    public Game
{
public:
    GravitationGame();
    ~GravitationGame();

    void Run() override;
    Matrix GetRandomRotateTransform();

    void Update(float deltaTime) override;
    void Render() override;

    InputHandler* inputHandler;

    float pressTime = 0.1f;
    float buttonTimer = 10.0f;

    UINT winWidth = 800;
    UINT winHeight = 800;

private:
    //Camera camera;
    std::vector<GravitationBody*> gravBodies;
    GravitationBody* focusedBody = nullptr;
    UINT focusedBodyIdx = 0;

    StarBox* starBox;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};

