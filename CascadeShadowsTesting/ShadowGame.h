#pragma once
#include <Game.h>
#include "TestCube.h"
#include <Camera.h>
#include <vector>
#include <Bindable.h>
#include "LightShadowStuff.h"
#include <ForwardRenderer.h>

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

    // camera

    struct Camera_PCB {
        XMFLOAT3 cam_pos;
    };
    Bind::PixelConstantBuffer<Camera_PCB>* cam_pcb;

    // Light

    LightData lightData;

    Vector3 lightPos;
    Bind::PixelConstantBuffer<LightData>* light_pcb;

    // Shadow
    UINT smSizeX = 512;
    UINT smSizeY = 512;
    //Camera* lightViewCameras[6];
    Camera* lightViewCamera;
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

