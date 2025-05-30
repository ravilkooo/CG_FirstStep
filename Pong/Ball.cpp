#include "Ball.h"

Ball::Ball()
{

	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	DirectX::XMFLOAT4 _points[8] = {
		DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	vertices = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	verticesNum = 8;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _points[i];
	}

	numInputElements = 2;

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
	IALayoutInputElements[0] =
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	IALayoutInputElements[1] =
		D3D11_INPUT_ELEMENT_DESC{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	vertexShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
	pixelShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Ball::Ball(DirectX::XMFLOAT4 position, float width) : position(position), width(width)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}

	width = width > 0 ? width : 0.01;
	DirectX::XMFLOAT4 _points[8] = {
		DirectX::XMFLOAT4(position.x + width, position.y + width, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + width,position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y + width, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	vertices = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	verticesNum = 8;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _points[i];
	}
	/*velocity_x = velocity_magn * DirectX::XMScalarCos(angle);
	velocity_y = velocity_magn * DirectX::XMScalarSin(angle);*/
	//velocity_magn = start_velocity;
	velocity = { -start_velocity, 0.f, 0.f, 0.f };

	numInputElements = 2;

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
	IALayoutInputElements[0] =
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	IALayoutInputElements[1] =
		D3D11_INPUT_ELEMENT_DESC{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	vertexShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
	pixelShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Ball::Ball(DirectX::XMFLOAT4* vertices)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}

	this->vertices = (DirectX::XMFLOAT4*)malloc(6 * sizeof(DirectX::XMFLOAT4));
	verticesNum = 6;

	for (int i = 0; i < 6; i++)
	{
		this->vertices[i] = vertices[i];
	}

	numInputElements = 2;

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
	IALayoutInputElements[0] =
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	IALayoutInputElements[1] =
		D3D11_INPUT_ELEMENT_DESC{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	vertexShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
	pixelShaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

void Ball::Update(float deltaTime)
{
	DirectX::XMMATRIX moveMat = DirectX::XMMatrixTranslationFromVector( DirectX::XMVectorScale(velocity, deltaTime));
	cb.wvpMat = cb.wvpMat * moveMat;

	//std::cout << "H\n";
}

void Ball::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
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

void Ball::Respawn()
{
	//velocity_magn = start_velocity;
	//angle = 0; // DirectX::XM_PIDIV4;
	//velocity_x = start_velocity * DirectX::XMScalarCos(angle);
	//velocity_y = start_velocity * DirectX::XMScalarSin(angle);
	velocity = { -start_velocity, 0.f, 0.f, 0.f };
	cb.wvpMat = DirectX::XMMatrixIdentity();
}

void Ball::GetCenterLocation(DirectX::XMFLOAT3* loc)
{
	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x + width * 0.5f,
		position.y + width * 0.5f,
		position.z, 1.0f);
	DirectX::XMStoreFloat3(loc, DirectX::XMVector4Transform(center, cb.wvpMat));
	return;
}

DirectX::BoundingBox Ball::GetBoundingBox() const
{
	DirectX::BoundingBox bbox;

	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x + width * 0.5f,
		position.y + width * 0.5f,
		position.z, 1.0f);
	DirectX::XMFLOAT3 extents(width * 0.5f,
		width * 0.5f,
		0.0f);	
	DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, cb.wvpMat));

	bbox.Extents = extents;
	return bbox;
}

DirectX::BoundingBox Ball::GetNextStepBoundingBox(float deltaTime) const
{
	DirectX::BoundingBox bbox;

	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x + width * 0.5f,
		position.y + width * 0.5f,
		position.z, 1.0f);
	DirectX::XMFLOAT3 extents(width * 0.5f,
		width * 0.5f,
		0.0f);
	//DirectX::XMMATRIX moveMat = DirectX::XMMatrixTranslation(velocity_x * deltaTime, velocity_y * deltaTime, 0);
	DirectX::XMMATRIX moveMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorScale(velocity, deltaTime));
	DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, cb.wvpMat * moveMat));

	bbox.Extents = extents;
	return bbox;
}
