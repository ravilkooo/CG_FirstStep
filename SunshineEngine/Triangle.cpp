#include "Triangle.h"

Triangle::Triangle()
{

	int _ind[3] = { 0, 1, 2 };
	indices = (int*)malloc(3 * sizeof(int));
	indicesNum = 3;
	for (int i = 0; i < 3; i++)
	{
		indices[i] = _ind[i];
	}


	DirectX::XMFLOAT4 _points[6] = {
		DirectX::XMFLOAT4(0.75f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.75f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
	};
	points = (DirectX::XMFLOAT4*)malloc(6 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 6;

	for (int i = 0; i < 6; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Triangle::Triangle(DirectX::XMFLOAT4* points)
{
	int _ind[3] = { 0, 1, 2 };
	indices = (int*)malloc(3 * sizeof(int));
	indicesNum = 3;
	for (int i = 0; i < 3; i++)
	{
		indices[i] = _ind[i];
	}

	this->points = (DirectX::XMFLOAT4*)malloc(6 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 6;

	for (int i = 0; i < 6; i++)
	{
		this->points[i] = points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

void Triangle::Update(float deltaTime)
{
	DirectX::XMMATRIX rotZMat = DirectX::XMMatrixTranslation(0.03f * deltaTime, 0.f, 0.0f);
	cb.wvpMat = cb.wvpMat * rotZMat;
}

void Triangle::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	ID3D11RenderTargetView* renderTargetView)
{
	context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	// 6. Create Set of Points
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cb, sizeof(cb));
	context->Unmap(pConstantBuffer, 0);

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, &(pVertexBuffer), strides, offsets);

	// 9. Set Vertex and Pixel Shaders
	context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	// 11. Set BackBuffer for Output
	context->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// 14. At the End of While (!isExitRequested): Draw the Triangle
	context->DrawIndexed(6, 0, 0);
}
