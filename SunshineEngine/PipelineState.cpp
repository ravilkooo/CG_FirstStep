#include "PipelineState.h"

PipelineState::PipelineState()
    : device(nullptr), context(nullptr), rasterizerState(nullptr), blendState(nullptr), depthStencilState(nullptr)
{
}

PipelineState::PipelineState(ID3D11Device* device, ID3D11DeviceContext* context)
    : device(device), context(context), rasterizerState(nullptr), blendState(nullptr), depthStencilState(nullptr)
{
    // Инициализация состояний по умолчанию
    //SetRasterizerState();
    //SetBlendState();
    //SetDepthStencilState();
}

PipelineState::~PipelineState()
{
    //if (rasterizerState) rasterizerState->Release();
    //if (blendState) blendState->Release();
    //if (depthStencilState) depthStencilState->Release();
}

void PipelineState::SetRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
{
    if (rasterizerState)
    {
        rasterizerState->Release();
        rasterizerState = nullptr;
    }
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = fillMode;
    rasterDesc.CullMode = cullMode;
 /*   rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;*/

    device->CreateRasterizerState(&rasterDesc, &rasterizerState);
    context->RSSetState(rasterizerState);
}

void PipelineState::SetBlendState(bool enableBlending)
{
    if (blendState)
    {
        blendState->Release();
        blendState = nullptr;
    }
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = enableBlending;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &blendState);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
}

// Unnecessary, cause it's set on default   
void PipelineState::SetDepthStencilState(bool enableDepthTest)
{
    /*if (depthStencilState)
    {
        depthStencilState->Release();
        depthStencilState = nullptr;
    }*/

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = enableDepthTest;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    //depthStencilDesc.StencilEnable = false;

    device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    context->OMSetDepthStencilState(depthStencilState, 1u);
}