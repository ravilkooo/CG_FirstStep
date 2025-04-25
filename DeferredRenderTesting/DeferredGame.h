#pragma once
#include <Game.h>
#include <DeferredRenderer.h>
#include <GBufferPass.h>
#include <LightPass.h>
#include <MainColorPass.h>

#include <PointLight.h>
#include <DirectionalLight.h>
#include "TestCube.h"
#include <FullScreenQuad.h>

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

