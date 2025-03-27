#pragma once
#include "Bindable.h"
namespace Bind
{
    class VertexBuffer :
        public Bindable
    {
    public:
        VertexBuffer(ID3D11Device* device, const void* data, UINT count, UINT stride);
        
        void Bind(ID3D11DeviceContext* context) noexcept override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
        UINT count;
        UINT stride;
    };

}
