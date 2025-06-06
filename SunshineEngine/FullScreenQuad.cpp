#include "FullScreenQuad.h"

FullScreenQuad::FullScreenQuad()
{
}

FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
    // MainColorPass
    {
        RenderTechnique* colorPass = new RenderTechnique("MainColorPass");
        colorPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));
        colorPass->AddBind(new Bind::VertexShader(device, L"./Shaders/MainColorPassVS.hlsl"));
        colorPass->AddBind(new Bind::PixelShader(device, L"./Shaders/MainColorPassPS.hlsl"));

        indices = (int*)calloc(4, sizeof(int));
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 3;

        indicesNum = 4;
        colorPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));

        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        colorPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        techniques.insert({ "MainColorPass", colorPass });
    }
}
