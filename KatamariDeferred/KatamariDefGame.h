#ifndef KATAMARIDEFGAME_H
#define KATAMARIDEFGAME_H

#include <Game.h>

#include <DeferredRenderer.h>
#include <GBufferPass.h>
#include <LightPass.h>
#include <MainColorPass.h>
//#include <DL_ShadowMapPass.h>
//#include <FrustrumWireframe.h>

#include <LightCollection.h>
#include <FullScreenQuad.h>

#include "StickyBall.h"
#include "Floor.h"
#include "CollectibleObject.h"




class KatamariDefGame :
    public Game
{
public:
    KatamariDefGame();
    ~KatamariDefGame();

    void Update(float deltaTime) override;
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    std::vector<CollectibleObject*> collectibles;
    void SpawnCollectibles();
    StickyBall* ball;
    Floor* floor;

    DirectionalLight* _dl_1;
    SpotLight* _sl_1;

    // int _randomPointLights = 6;
    //std::vector<Vector3> pointLightSpeeds;
    //std::vector<Vector3> pointLightAccel;
    //std::vector<PointLight*> pointLights;
    

    UINT instancedCnt = 1000;
    PointLightInstanced* instancedPointLights;
    /*
    ID3D11Buffer* pointLightsBuffer;
    */


    /*
    XMFLOAT3 pointLightColors[8];
    XMFLOAT3 pointLightDirections[8];
    float pointLightLifeTime[8];
    float lifeTime = 2.0f;
    size_t currPointLightBullet = 0;
    */

    ParticleSystem* dustParticleSystem;
    ParticleSystem* starParticleSystem;
    ParticleSystem* bubbleParticleSystem;
    Vector3 bubbleFlowDirection = { 1, 0, 0 };
    // Shadow
    //Bind::PixelConstantBuffer<DL_ShadowMapPass::CascadesData>* cascadesConstantBuffer;
    
    LightPass* gLightPass;

    Bind::TextureB* normalMap;
    Bind::TextureB* worldPosMap;

    Bind::Sampler* sam;

    struct viewProjectionBuffer
    {
        Matrix vMat;
        Matrix pMat;
    };

    Bind::ComputeConstantBuffer<viewProjectionBuffer>* particlesViewProjMat;

};

#endif
