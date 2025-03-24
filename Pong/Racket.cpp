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


	Vertex _vertices[4] = {
	  DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f),  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
	  DirectX::XMFLOAT3(-0.25f, -0.25f, 0.25f),  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
	  DirectX::XMFLOAT3(0.25f, -0.25f, 0.25f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	  DirectX::XMFLOAT3(-0.25f, 0.25f, 0.25f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	vertices = (Vertex*) malloc(4 * sizeof(Vertex));
	verticesNum = 4;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _vertices[i];
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
	Vertex _vertices[8] = {
	  DirectX::XMFLOAT3(position.x + width * .5f, position.y + height * .5f, position.z + 0.0f),  color,
	  DirectX::XMFLOAT3(position.x - width * .5f, position.y - height * .5f, position.z + 0.0f),  color,
	  DirectX::XMFLOAT3(position.x + width * .5f, position.y - height * .5f, position.z + 0.0f),  color,
	  DirectX::XMFLOAT3(position.x - width * .5f, position.y + height * .5f, position.z + 0.0f),  color,
	};
	vertices = (Vertex*)malloc(8 * sizeof(Vertex));
	verticesNum = 8;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _vertices[i];
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

void Racket::Update(float deltaTime)
{
	curr_angle += angle_velocity * deltaTime;
	if (curr_angle > DirectX::XM_PI) curr_angle = -DirectX::XM_PI;
	DirectX::XMMATRIX moveMat = DirectX::XMMatrixTranslation(velocity_magn_x * deltaTime, velocity_magn_y * deltaTime, 0.f);
	
	DirectX::XMFLOAT3 center_pos;
	GetCenterLocation(&center_pos);
	DirectX::XMMATRIX toCenter = DirectX::XMMatrixTranslation(-center_pos.x, -center_pos.y, -center_pos.z);
	DirectX::XMMATRIX fromCenter = DirectX::XMMatrixTranslation(center_pos.x, center_pos.y, center_pos.z);

	DirectX::XMMATRIX roteMat = DirectX::XMMatrixRotationZ(angle_velocity * deltaTime);
	
	normal = DirectX::XMVector3Transform(normal, roteMat);
	Normalize();
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

void Racket::HitBall(Ball* ball, float deltaTime)
{
	//ball->velocity_x *= -1;
	float velocity_magn_y = DirectX::XMVectorGetX(DirectX::XMVector3Length(ball->velocity));
	ball->velocity = DirectX::XMVectorScale(ball->velocity, (velocity_magn_y + ball->velocity_step) / velocity_magn_y);

	/*DirectX::XMFLOAT3 bc;
	ball->GetCenterLocation(&bc);
	DirectX::XMFLOAT3 rc;
	GetCenterLocation(&rc);

	float b = bc.y - rc.y;
	float beta = asin(0.25 * b / height);*/
	
	/*float beta = curr_angle;
	float new_angle = max(-0.95 * DirectX::XM_PIDIV2, min(2 * beta + ball->angle, 0.95*DirectX::XM_PIDIV2));
	
	ball->angle = abs(new_angle);
	std::cout << new_angle << "\n";
	std::cout << ball->velocity_y << "\n";

	ball->velocity_x = ball->velocity_magn * cos(ball->angle);
	ball->velocity_y = ball->velocity_magn * sin(ball->angle);
	*/
	
	//float new_v_x = ball->velocity_x - 2 * rav

	float _dir = isFaced(ball->velocity) ? 1.f : -1.f;
	
	DirectX::XMFLOAT3 bc;
	ball->GetCenterLocation(&bc);
	DirectX::XMFLOAT3 rc;
	GetCenterLocation(&rc);
	DirectX::XMVECTOR r2b = { bc.x - rc.x, bc.y - rc.y, bc.z - rc.z, 0 }; // надо повернуть
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationZ(-curr_angle);
	r2b = DirectX::XMVector3Transform(r2b, rotMat);

	float b = DirectX::XMVectorGetY(r2b);
	DirectX::XMVECTOR _normal = DirectX::XMVECTOR(normal);
	DirectX::XMVECTOR curr_normal;
	if (abs(b) > height * 0.5) {
		float a = abs(DirectX::XMVectorGetX(r2b));
		curr_normal = NormalDistribution(a);
		DirectX::XMMATRIX _90rotMat = DirectX::XMMatrixRotationZ(DirectX::XM_PIDIV2 * (b > 0 ? 1 : -1));
		curr_normal = DirectX::XMVector3Transform(curr_normal, _90rotMat);
		_normal = DirectX::XMVector3Transform(_normal, _90rotMat);
	}
	{
		curr_normal = NormalDistribution(b);
	}
	//std::cout << DirectX::XMVectorGetX(curr_normal) << ", " << DirectX::XMVectorGetY(curr_normal) << "\n";

	DirectX::XMVECTOR old_v = DirectX::XMVECTOR(ball->velocity);

	ball->velocity = ReflectVector(curr_normal, ball->velocity);


	if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(_normal, ball->velocity)) < 0) {
		//std::cout << DirectX::XMVectorGetX(_normal) << ", " << DirectX::XMVectorGetY(_normal) << " :::: ";
		//std::cout << DirectX::XMVectorGetX(ball->velocity) << ", " << DirectX::XMVectorGetY(ball->velocity) << "\n";

		ball->velocity = ReflectVector(_normal, old_v);
		/*ball->velocity = DirectX::XMVectorScale(
			DirectX::XMVector3Normalize(
				DirectX::XMVectorSubtract(ball->velocity,
					DirectX::XMVectorScale(normal,
						DirectX::XMVectorGetX(DirectX::XMVector3Dot(ball->velocity, normal))))),
			DirectX::XMVectorGetX(DirectX::XMVector3Length(ball->velocity)));*/
		DirectX::XMMATRIX deltaRotMat = DirectX::XMMatrixRotationZ(angle_velocity * deltaTime * 5.f);
		ball->velocity = DirectX::XMVector3Transform(ball->velocity, deltaRotMat);
	}
}

void Racket::Move(float velocity_magn_y) {
	this->velocity_magn_y = velocity_magn_y;
}

DirectX::BoundingOrientedBox Racket::GetBoundingBox() const
{
	DirectX::BoundingOrientedBox bbox;

	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
		position.y,
		position.z, 1.0f);
	DirectX::XMFLOAT3 extents(width * 0.5f,
		height * 0.5f,
		0.0f);
	DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, cb.wvpMat));

	bbox.Extents = extents;
	DirectX::XMStoreFloat4(&(bbox.Orientation), DirectX::XMQuaternionRotationRollPitchYaw(0.f, 0.f, curr_angle));
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
	velocity_magn_y = max_velocity;
}

void Racket::MoveDown()
{
	velocity_magn_y = -max_velocity;
}

void Racket::MoveLeft()
{
	velocity_magn_x = -max_velocity;
}

void Racket::MoveRight()
{
	velocity_magn_x = max_velocity;
}

void Racket::Stop()
{
	velocity_magn_x = 0;
	velocity_magn_y = 0;
}

void Racket::Normalize() {
	normal = DirectX::XMVector3Normalize(normal);
}

DirectX::XMVECTOR Racket::ReflectVector(const DirectX::XMVECTOR& normalVec, const DirectX::XMVECTOR& incidentVec) {
	return DirectX::XMVector3Reflect(incidentVec, normalVec);
}

bool Racket::isFaced(const DirectX::XMVECTOR& v) {
	return DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, v)) < 0;
}

DirectX::XMVECTOR Racket::NormalDistribution(float value) {
	return normal;
	float _s = 0.2; // < 0.5
	float sin_beta = max(0.0f, min(_s * value / height, 0.9)); // < 1
	float cos_beta = sqrt(1 - sin_beta * sin_beta); // < 1
	
	//return DirectX::XMLoadFloat4(&(DirectX::XMFLOAT4( cos_beta, sin_beta, .0f, 0.f )));
	return { cos_beta, sin_beta, .0f, 0.f };
}
