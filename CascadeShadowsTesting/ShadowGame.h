#pragma once
#include <Game.h>
#include "TestCube.h"
#include <Camera.h>
#include <vector>
#include <Bindable.h>
#include "LightShadowStuff.h"

class ShadowGame :
    public Game
{
public:
    ShadowGame();
    ~ShadowGame();

    void Run() override;
    void Update(float deltaTime) override;
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    std::vector<TestCube*> shadowableObjects;

    // Light

    LightData lightData;

    Vector3 lightPos = { 0, 0, -2 };
    Bind::PixelConstantBuffer<LightData>* light_pcb;

    // Shadow
    Camera* lightViewCamera;
    UINT smSizeX = 400;
    UINT smSizeY = 400;
    ID3D11Texture2D* shadowTexture;
    ID3D11DepthStencilView* depthDSV;
    ID3D11ShaderResourceView* depthSRV;
    D3D11_VIEWPORT smViewport;
    
    void BindDsvAndSetNullRenderTarget();

    void DrawSceneToShadowMap();

    Bind::VertexShader* shadowVShader;
    Bind::InputLayout* shadowInpLayout;
    // Bind::VertexBuffer* shadowVBuffer;
    Bind::IndexBuffer* shadowIBuffer;
    Bind::Rasterizer* shadowRast;

    Bind::VertexConstantBuffer<ShadowTransform>* shadowTransforms;
};

