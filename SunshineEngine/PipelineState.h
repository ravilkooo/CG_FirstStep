#ifndef PIPELINESTATE_H
#define PIPELINESTATE_H

#include <d3d11.h>
#include <wrl.h>

class PipelineState
{
public:
    PipelineState();
    PipelineState(ID3D11Device* device, ID3D11DeviceContext* context);
    ~PipelineState();
    void ClearState() {
        context->ClearState();
    }
    void SetRasterizerState(D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID);
    void SetBlendState(bool enableBlending = false);
    void SetDepthStencilState(bool enableDepthTest = true);

private:
    ID3D11DepthStencilState* depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* context;

    ID3D11RasterizerState* rasterizerState;
    ID3D11BlendState* blendState;
};

#endif // PIPELINESTATE_H