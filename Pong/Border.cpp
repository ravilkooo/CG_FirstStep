#include "Border.h"

Border::Border()
{

	int _ind[12] = { 0, 1, 2, 1, 0, 3,
		4, 5, 6, 5, 4, 7 };
	indices = (int*)malloc(12 * sizeof(int));
	indicesNum = 12;
	for (int i = 0; i < 12; i++)
	{
		indices[i] = _ind[i];
	}


	DirectX::XMFLOAT4 _points[16] = {
		DirectX::XMFLOAT4(1.f, 1.f + height, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(-1.f, 1.f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(1.f, 1.f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(-1.f, 1.f + height, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(1.f, -1.f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(-1.f, -1.f - height, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(1.f, -1.f - height, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
		DirectX::XMFLOAT4(-1.f, -1.f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f),
	};
	points = (DirectX::XMFLOAT4*)malloc(16 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 16;

	for (int i = 0; i < 16; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

//Border::Border(float width, float height)
//{
//	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
//	indices = (int*)malloc(6 * sizeof(int));
//	indicesNum = 6;
//	for (int i = 0; i < 6; i++)
//	{
//		indices[i] = _ind[i];
//	}
//
//
//	width = width > 0 ? width : 0.01;
//	height = height > 0 ? height : 0.01;
//	DirectX::XMFLOAT4 _points[8] = {
//		DirectX::XMFLOAT4(position.x + width, position.y + height, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT4(position.x, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT4(position.x + width, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT4(position.x, position.y + height, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
//	};
//	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
//	pointsNum = 8;
//
//	for (int i = 0; i < 8; i++)
//	{
//		points[i] = _points[i];
//	}
//
//	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
//}

void Border::Update(float deltaTime)
{
	/*DirectX::XMMATRIX rotZMat = DirectX::XMMatrixRotationZ(0.3f * deltaTime);
	cb.wvpMat = cb.wvpMat * rotZMat;*/

	//std::cout << "H\n";
}

void Border::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
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
	context->DrawIndexed(12, 0, 0);
}

void Border::HitBall(Ball* ball)
{
	ball->velocity = DirectX::XMVector3Reflect(ball->velocity, normal);
}

DirectX::BoundingBox* Border::GetBoundingBoxes() const
{
	DirectX::BoundingBox bbox_1;
	DirectX::BoundingBox bbox_2;

	DirectX::XMVECTOR center_1 = DirectX::XMVectorSet(0.f, 1.f + height * 0.5f, 0.25f, 1.0f);
	DirectX::XMVECTOR center_2 = DirectX::XMVectorSet(0.f, -1.f - height * 0.5f, 0.25f, 1.0f);

	DirectX::XMFLOAT3 extents(1.f, 0.5f * height, 0.0f);
	DirectX::XMStoreFloat3(&(bbox_1.Center), DirectX::XMVector4Transform(center_1, cb.wvpMat));
	DirectX::XMStoreFloat3(&(bbox_2.Center), DirectX::XMVector4Transform(center_2, cb.wvpMat));

	bbox_1.Extents = extents;
	bbox_2.Extents = extents;

	DirectX::BoundingBox bbox[2] = { bbox_1, bbox_2 };
	return bbox;
}