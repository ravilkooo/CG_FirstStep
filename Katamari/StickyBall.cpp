#include "StickyBall.h"

StickyBall::StickyBall()
{
}
StickyBall::StickyBall(ID3D11Device* device)
{
	this->device = device;
	UINT sliceCount = 15;
	UINT elevationCount = 8;
	XMFLOAT4 col(0.9f, 0.0f, 0.0f, 1.0f);
	CreateSimpleSphereMesh(radius, sliceCount, elevationCount, col, &vertices, &verticesNum, &indices, &indicesNum);
	for (size_t i = 0; i < verticesNum; i++)
	{
		vertices[i].pos = Vector3::Transform(vertices[i].pos, Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, XM_PIDIV2));
	}
	//auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
	auto col_2 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < elevationCount * 2 + 1; i++)
	{
		for (size_t j = 0; j < sliceCount; j+=2)
		{
			vertices[1 + i * sliceCount + j].color = col_2;
		}
	}
	worldMat = Matrix::CreateTranslation(position);

	numInputElements = 3;

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

	IALayoutInputElements[2] =
		D3D11_INPUT_ELEMENT_DESC{
			"TEXCOORD",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT, // 28,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	vertexShaderFilePath = L"./Shaders/StickyBallVShader.hlsl";
	pixelShaderFilePath = L"./Shaders/StickyBallPShader.hlsl";
	this->textures.push_back(Texture(device, "models\\Textures\\pixeleye.dds", aiTextureType_DIFFUSE));
	hasTexture = true;
	
	// delete this
}

Vector3 StickyBall::GetCenterLocation()
{
	Vector3 center = Vector3(position);
	return center;
}

void StickyBall::PushForward(float deltaTime)
{
	velocity = min(maxVelocity, velocity + deltaTime * 2 * acceleration);
	spinSpeed = velocity / radius;
}

void StickyBall::AddTurn(float direction, float deltaTime)
{
	currentRotation += direction * deltaTime * rotationSpeed;
	//currentRotation = max(-XM_PI, min(currentRotation, XM_PI));
	if (currentRotation > XM_PI) {
		currentRotation -= XM_2PI;
	}
	else if (currentRotation < -XM_PI) {
		currentRotation += XM_2PI;
	}
}

void StickyBall::SlowDown(float deltaTime)
{
	velocity = max(0.0, velocity - deltaTime * acceleration);
	spinSpeed = velocity / radius;
}

Vector3 StickyBall::GetMoveDir()
{
	return Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f));
}

void StickyBall::Update(float deltaTime)
{

	//curr_rotation = 0.0f;
	radiusGrow = max(0.0, radiusGrow - deltaTime * radiusSlow);
	radius += radiusGrow * deltaTime;

	position.y = radius;
	position = position + deltaTime * velocity * GetMoveDir();
	currentSpin += spinSpeed * deltaTime;



	Matrix spinTransform = Matrix::CreateFromYawPitchRoll(0.0f, currentSpin, 0.0f);
	Matrix rotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f);
	worldMat = Matrix::CreateScale(radius) * spinTransform * rotTransform * Matrix::CreateTranslation(position);


	//Matrix viewMat = camera->GetViewMatrix();
	//Matrix projMat = camera->GetProjectionMatrix();

	//cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}

void StickyBall::UpdateScale()
{
    worldMat = Matrix::CreateScale(radius) * 
             Matrix::CreateFromYawPitchRoll(currentRotation, 0, currentSpin) * 
             Matrix::CreateTranslation(position);
}

void StickyBall::Grow(float volume)
{
	radiusGrow = 0.5f / sqrt(radius);
}
