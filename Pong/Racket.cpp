#include "Racket.h"

Racket::Racket()
{

	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	DirectX::XMFLOAT4 _points[8] = {
	  DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
	  DirectX::XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
	  DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	  DirectX::XMFLOAT4(-0.25f, 0.25f, 0.25f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 8;

	for (int i = 0; i < 8; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Racket::Racket(DirectX::XMFLOAT4 position, float width) :
	Racket(position, width, width)
{
}

Racket::Racket(DirectX::XMFLOAT4 position, float width, float height) :
	Racket(position, width, height, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f))

{

}

Racket::Racket(DirectX::XMFLOAT4 position, float width, float height, DirectX::XMFLOAT4 color)
	: position(position), width(width), height(height)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	width = width > 0 ? width : 0.01;
	height = height > 0 ? height : 0.01;
	DirectX::XMFLOAT4 _points[8] = {
	  DirectX::XMFLOAT4(position.x + width * .5f, position.y + height * .5f, position.z + 0.0f, position.w + 0.0f),  color,
	  DirectX::XMFLOAT4(position.x - width * .5f, position.y - height * .5f, position.z + 0.0f, position.w + 0.0f),  color,
	  DirectX::XMFLOAT4(position.x + width * .5f, position.y - height * .5f, position.z + 0.0f, position.w + 0.0f),  color,
	  DirectX::XMFLOAT4(position.x - width * .5f, position.y + height * .5f, position.z + 0.0f, position.w + 0.0f),  color,
	};
	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 8;

	for (int i = 0; i < 8; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Racket::Racket(DirectX::XMFLOAT4* points)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
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

void Racket::Update(float deltaTime)
{
	DirectX::XMMATRIX moveMat = DirectX::XMMatrixTranslation(0.f, velocity * deltaTime, 0.f);
	
	DirectX::XMFLOAT3 center_pos;
	GetCenterLocation(&center_pos);
	DirectX::XMMATRIX toCenter = DirectX::XMMatrixTranslation(-center_pos.x, -center_pos.y, -center_pos.z);
	DirectX::XMMATRIX fromCenter = DirectX::XMMatrixTranslation(center_pos.x, center_pos.y, center_pos.z);

	DirectX::XMMATRIX roteMat = DirectX::XMMatrixRotationZ(angle_velocity * deltaTime);

	cb.wvpMat = cb.wvpMat * toCenter * roteMat * fromCenter * moveMat;
}

void Racket::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
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

void Racket::HitBall(Ball* ball)
{
	ball->direction_x *= -1;
	ball->velocity += ball->velocity_step;

	DirectX::XMFLOAT3 bc;
	ball->GetCenterLocation(&bc);
	DirectX::XMFLOAT3 rc;
	GetCenterLocation(&rc);

	float b = bc.y - rc.y;
	float beta = asin(0.25 * b / height);
	ball->angle = min(abs(2 * beta - ball->angle), DirectX::XM_PIDIV2*0.95f);

	ball->velocity_x = ball->velocity * cos(ball->angle);
	ball->velocity_y = ball->velocity * sin(ball->angle);
}

void Racket::Move(float velocity) {
	this->velocity = velocity;
}

DirectX::BoundingBox Racket::GetBoundingBox() const
{
	DirectX::BoundingBox bbox;

	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
		position.y,
		position.z, 1.0f);
	DirectX::XMFLOAT3 extents(width * 0.5f,
		height * 0.5f,
		0.0f);
	DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, cb.wvpMat));

	bbox.Extents = extents;
	return bbox;
}

void Racket::GetCenterLocation(DirectX::XMFLOAT3* loc)
{
	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
		position.y,
		position.z, 1.0f);
	DirectX::XMStoreFloat3(loc, DirectX::XMVector4Transform(center, cb.wvpMat));
	return;
}

void Racket::MoveUp()
{
	velocity = max_velocity;
}

void Racket::MoveDown()
{
	velocity = -max_velocity;
}

void Racket::Stop()
{
	velocity = 0;
}
