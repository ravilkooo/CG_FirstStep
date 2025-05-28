#ifndef KATAMARIGAME_H
#define KATAMARIGAME_H

#include <Game.h>

#include <ForwardRenderer.h>
#include <MainColorPass.h>
#include <DL_ShadowMapPass.h>

#include <FrustrumWireframe.h>

#include "StickyBall.h"
#include "Floor.h"
#include "CollectibleObject.h"




class KatamariGame :
    public Game
{
public:
    KatamariGame();
    ~KatamariGame();

    void Run() override;
    void Update(float deltaTime) override;
    void Render() override;

    std::vector<CollectibleObject*> collectibles;
    void SpawnCollectibles();

    InputHandler* inputHandler;

    StickyBall* ball;
    Floor* floor;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    Bind::PixelConstantBuffer<LightData_old>* light_pcb;

    LightData_old lightData;
    XMFLOAT3 pointLightColors[8];
    XMFLOAT3 pointLightDirections[8];
    float pointLightLifeTime[8];
    float lifeTime = 2.0f;
    size_t currPointLightBullet = 0;

    // Shadow
    DL_ShadowMapPass* dl_shadowMapPass;

    Bind::TextureB* backgroundTexture;
};

#endif
