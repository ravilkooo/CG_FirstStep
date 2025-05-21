#pragma once
#include <stdexcept>

#include "RenderPass.h"
#include "Camera.h"
#include "GBuffer.h"
#include "LightCollection.h"

// temp
#include "ParticleSystem.h"

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

        XMFLOAT3 velocity;
        float energy;
    };
    UINT numParticles = 1024;
    std::vector<Particle> particles;

    // Буферы
    ID3D11Buffer* particlePool;          // Буфер частиц (StructuredBuffer)
    ID3D11ShaderResourceView* particleSRV;
    ID3D11UnorderedAccessView* uavParticlePool;

    ID3D11Buffer* deadList;              // Список мертвых частиц (ConsumeStructuredBuffer)
    ID3D11UnorderedAccessView* uavDeadList;

    ID3D11Buffer* sortList;              // Список для сортировки (AppendStructuredBuffer)
    ID3D11UnorderedAccessView* uavSortList;

    ID3D11ComputeShader* emitComputeShader = nullptr;
    ID3D11ComputeShader* simulateComputeShader = nullptr;

    ID3D11Buffer* cs_cb;
    ID3D11Buffer* emitConstantBuffer;
    

    ID3D11Buffer* indirectArgsBuffer;
    Bind::VertexConstantBuffer<XMMATRIX>* ps_vcb;
    Bind::IndexBuffer* ps_ib;
    Bind::VertexShader* ps_vs;
    Bind::PixelShader* ps_ps;
    Bind::InputLayout* ps_ia;
    float currTime = 0.0f;

    struct EmitConstants {
        UINT numParticlesToEmit;
        float padding[3]; // Выравнивание до 16 байт
    };


public:
    float emissionRate = 1.0f;    // Частиц в секунду
    float accumulatedTime = 0.0f;  // Накопленное время для эмиссии

    UINT numNewParticles = 0;
    
    // temp
    // actual particle system
    ParticleSystem particleSystem;
    
};

