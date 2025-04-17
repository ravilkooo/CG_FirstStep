#pragma once
#include <Game.h>
#include <DeferredRenderer.h>
#include <GBufferPass.h>
#include <MainColorPass.h>

#include "TestCube.h"

class DeferredGame :
    public Game
{
public:
    DeferredGame();
    ~DeferredGame();

    void Update(float deltaTime) override;
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};

