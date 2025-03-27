#include "VertexBuffer.h"

Bind::VertexBuffer::VertexBuffer(ID3D11Device* device, const void* data, UINT count, UINT stride)
	: count(count), stride(stride)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.ByteWidth = count * stride;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = data;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bd, &InitData, &pVertexBuffer);
}

void Bind::VertexBuffer::Bind(ID3D11DeviceContext* context) noexcept
{
    const UINT offset = 0u;
    context->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
