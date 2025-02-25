#include "InputAssembler.h"

InputAssembler::InputAssembler()
{
}

InputAssembler::InputAssembler(ID3D11Device* device, ID3D11DeviceContext* context) : device(device), context(context)
{
	layout = nullptr;
}

InputAssembler::~InputAssembler()
{
}

void InputAssembler::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* inputElements, ID3DBlob* vsBlob)
{
	device->CreateInputLayout(
		inputElements,
		2,
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&layout);

	//vsBlob->Release();
}

void InputAssembler::SetVertexBuffer(ID3D11Buffer* vertexBuffer, UINT stride, UINT offset)
{
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
}

void InputAssembler::SetIndexBuffer(ID3D11Buffer* indexBuffer)
{
}

void InputAssembler::SetInputLayout()
{
	context->IASetInputLayout(layout.Get());
}


void InputAssembler::SetInputLayout(ID3D11InputLayout* inputLayout)
{
    context->IASetInputLayout(inputLayout);
}

void InputAssembler::Release()
{
	layout->Release();
}

void InputAssembler::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    context->IASetPrimitiveTopology(topology);
}