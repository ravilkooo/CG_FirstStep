#pragma once

#include <d3d11.h>

#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <string>

#include "BindableCollection.h"

#include "Camera.h"

class ParticleSystem
{
    // Emmit
    // indirectDispatchArgs[m_currentAliveBuffer] - count new spawned particles
    // aliveParticleIndex[m_currentAliveBuffer] - new spawned particles

    // Simulate-Init : Dispatch(1, 1, 1)
    // m_indirectDispatchArgs[m_currentAliveBuffer] :=
    //      { m_indirectDispatchArgs / nbThreadGroupX, 1, 1 }
    // partilces per thread/group?
    

    // m_aliveListCountConstantBuffer := Counter(m_aliveIndex[m_currentAliveBuffer])
    
    // Simulate : DispatchIndirect(m_indirectDispatchArgs[m_currentAliveBuffer])
    // m_aliveListCountConstantBuffer - as ConstBuffer
    // m_aliveIndex[m_currentAliveBuffer] - alive particles before simulating
    // m_aliveIndex[(m_currentAliveBuffer + 1) % 2] - alive particles after simulating
    // m_indirectDispatchArgs[(m_currentAliveBuffer + 1) % 2] - count alive particles after simulating (args for dispatch m_simulateShader)
    // m_indirectDrawArgs - count alive particles after simulating (args for drawing)

    // m_aliveListCountConstantBuffer := Counter(m_aliveIndex[(m_currentAliveBuffer + 1) % 2])

    // m_currentAliveBuffer = (m_currentAliveBuffer + 1) % 2;
    
    // DrawInstancedIndirect(m_indirectDrawArgs)
    // m_aliveListCountConstantBuffer - as CB
    // m_aliveIndex[m_currentAliveBuffer]  - as SRV



public:

    struct Particle {
        DirectX::XMFLOAT4 Position;
        DirectX::XMFLOAT4 Velocity;
        DirectX::XMFLOAT4 Color;
        float Size;
        float Age;
        float lifeSpan;
        float padding;
    };

    struct ParticleIndexElement
    {
        float distance; //squared distance from camera
        float index; //index in the particle buffer
    };

    struct InitIndirectComputeArgs1DConstantBuffer
    {
        float nbThreadGroupX;

        UINT padding[3];
    };

    int m_maxParticles = 4 * 1024;


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particleBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleSRV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_particleUAV;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_deadListUAV;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveIndexBuffer[2];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_aliveIndexSRV[2];
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAV[2];
    // Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAVSorting[2];
    int                                                 m_currentAliveBuffer = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDispatchArgsBuffer[2];    // для комп шейдера
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDispatchArgsUAV[2];       // для рендеринга


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_initSimulateDispatchArgsBuffer;
    InitIndirectComputeArgs1DConstantBuffer             m_initSimulateDispatchArgsData;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDrawArgsBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDrawArgsUAV;


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveListCountConstantBuffer;

    ParticleSystem() {};
    ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* context);
    void LoadCS(LPCWSTR computeFilename, ID3D11ComputeShader* m_computeShader);
    void ResetParticles();

    void Update(float deltaTime);
    void UpdateEmitter(float deltaTime);

    void Render();
    void Emit();
    void Simulate();
    void Draw();

    void SetEmissionRate(float emissionRate) { m_emissionRate = max(0, min(emissionRate, m_maxParticles * 1. / 4)); };
    void IncrementEmissionRate(float deltaEmissionRate) { SetEmissionRate(m_emissionRate + deltaEmissionRate); };
    void DecrementEmissionRate(float deltaEmissionRate) { SetEmissionRate(m_emissionRate - deltaEmissionRate); };


    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_resetCShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_initSimulateDispatchArgsCShader; // pre-simulate
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_emitParticlesCShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_simulateParticlesCShader;

    Microsoft::WRL::ComPtr<ID3D11Device>               m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>        m_d3dContext;

    Camera* camera;

private:
    struct DeadListCountConstantBuffer
    {
        UINT nbDeadParticles;

        UINT padding[3];
    };

    float m_emissionRate = 100.0f;
    float m_emissionRateAccumulation = 0.0f;

    struct EmitterPointConstantBuffer
    {
        Vector4 position;
        UINT maxSpawn;
        float particlesLifeSpan;
        UINT padding[2];
    };

    EmitterPointConstantBuffer                         m_emitterConstantBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

    inline int align(int value, int alignment) { return (value + (alignment - 1)) & ~(alignment - 1); };

    // temp

    struct SceneConstantBuffer
    {
        Vector4 camPosition;
        float dt;
        float rngSeed;
        UINT padding[2];
    };
    SceneConstantBuffer m_sceneConstantBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_sceneConstantBuffer;

    ID3D11RasterizerState* rasterState;
    ID3D11DepthStencilState* depthState;
    Bind::BlendState* m_BlendState;
    struct TransformsParticles
    {
        DirectX::XMMATRIX viewMat;
        DirectX::XMMATRIX projMat;
        DirectX::XMMATRIX viewProjMat;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjBuffer;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>       pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_renderParticleVS;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_renderParticleGS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_renderParticlePS;

    // texture
    Bind::TextureB* texture;
    Bind::Sampler* textureSampler;
    //colorPass->AddBind(new Bind::TextureB(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE, 1u));
};

