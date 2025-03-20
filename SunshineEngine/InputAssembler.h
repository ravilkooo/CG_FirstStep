#ifndef INPUTASSEMBLER_H
#define INPUTASSEMBLER_H

#include <d3d11.h>
#include <wrl.h>

class InputAssembler
{
public:
    InputAssembler();
    InputAssembler(ID3D11Device* device, ID3D11DeviceContext* context);
    ~InputAssembler();

    void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* inputElements, UINT numInputElements, ID3DBlob* vsBlob);

    void SetVertexBuffer(ID3D11Buffer* vertexBuffer, UINT stride, UINT offset);
    void SetIndexBuffer(ID3D11Buffer* indexBuffer);
    void SetInputLayout();
    void SetInputLayout(ID3D11InputLayout* inputLayout);
    void Release();
    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* context;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
};

#endif // INPUTASSEMBLER_H