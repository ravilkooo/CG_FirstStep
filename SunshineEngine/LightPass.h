#pragma once
#include <stdexcept>

#include "RenderPass.h"
#include "Camera.h"
#include "GBuffer.h"
#include "LightCollection.h"

class LightPass :
    public RenderPass
{
public:
    LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    GBuffer* pGBuffer;

    ID3D11RenderTargetView* gBufferRTV;
    D3D11_VIEWPORT viewport;

    struct CamPCB {
        XMMATRIX viewMatInverse;
        XMMATRIX projMatInverse;
        XMFLOAT3 camPos;
        float pad;
    } cameraData;
    Bind::PixelConstantBuffer<CamPCB>* camPCB;

private:
    // particle test
    struct Particle {
        DirectX::XMFLOAT3 Position;
        float Size;
        DirectX::XMFLOAT4 Color;
    };
    UINT numParticles = 1000;
    std::vector<Particle> particles;

    ID3D11Buffer* particleBuffer;
    ID3D11ShaderResourceView* particleSRV;
    ID3D11Buffer* indirectArgsBuffer;
    Bind::VertexConstantBuffer<XMMATRIX>* ps_vcb;
    Bind::IndexBuffer* ps_ib;
    Bind::VertexShader* ps_vs;
    Bind::PixelShader* ps_ps;
    Bind::InputLayout* ps_ia;
    float currTime = 0.0f;
};

