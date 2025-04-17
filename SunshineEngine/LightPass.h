#pragma once
#include "RenderPass.h"
#include "Camera.h"
#include "GBuffer.h"

class LightPass :
    public RenderPass
{
public:
    LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer);

    void StartFrame() override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    GBuffer* pGBuffer;
};

