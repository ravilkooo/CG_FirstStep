#pragma once
#include <stdexcept>

#include "RenderPass.h"
#include "Camera.h"
#include "GBuffer.h"

class GBufferPass :
    public RenderPass
{
public:
    GBufferPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight);

    // Inherited via RenderPass
    void StartFrame() override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    //ID3D11RenderTargetView* renderTargetView;
    ID3D11RenderTargetView* gBufferRTVs[3];
    D3D11_VIEWPORT viewport;
    //ID3D11Texture2D* pDepthStencil;
    //ID3D11DepthStencilView* pDSV;
    GBuffer* pGBuffer;

};

