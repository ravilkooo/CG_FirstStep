#pragma once
#include <Game.h>
#include <DeferredRenderer.h>
#include <GBufferPass.h>
#include <LightPass.h>
#include <MainColorPass.h>

#include <LightCollection.h>
#include "TestCube.h"
#include <FullScreenQuad.h>
#define DEBUG_LIGHT_OBJECTS

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
    SpotLight* _sl_1;
    float currTime = 0.0f;
    DirectionalLight* _dl_1;
};

