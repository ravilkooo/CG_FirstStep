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
	/*
	for (size_t i = 0; i < verticesNum; i++)
	{
		vertices[i].pos = Vector3::Transform(vertices[i].pos, Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, XM_PIDIV2));
	}
	*/
	//auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
	/*auto col_2 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < elevationCount * 2 + 1; i++)
	{
		for (size_t j = 0; j < sliceCount; j+=2)
		{
			vertices[1 + i * sliceCount + j].color = col_2;
		}
	}
	*/
	worldMat = Matrix::CreateTranslation(position);

	// GBufferPass
	{
		RenderTechnique* gBufferPass = new RenderTechnique("GBufferPass");
		gBufferPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		gBufferPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
		gBufferPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
		auto vertexShaderB = new Bind::VertexShader(device, L"./Shaders/GBufferPass/StickyBallGBufferShaderVS.hlsl");
		gBufferPass->AddBind(vertexShaderB);
		gBufferPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));


		numInputElements = 4;
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
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };
		IALayoutInputElements[3] =
			D3D11_INPUT_ELEMENT_DESC{
				"NORMAL",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };

		gBufferPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));

		gBufferPass->AddBind(new Bind::PixelShader(device, L"./Shaders/GBufferPass/StickyBallGBufferShaderPS.hlsl"));
		gBufferPass->AddBind(new Bind::TextureB(device, "./models/Textures/pixeleye.dds", aiTextureType_DIFFUSE, 0u));
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		gBufferPass->AddBind(new Bind::Sampler(device, samplerDesc, 0u));



		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		gBufferPass->AddBind(new Bind::Rasterizer(device, rastDesc));

		techniques.insert({ "GBufferPass", gBufferPass });
	}
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
	return Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), Matrix::CreateRotationY(currentRotation));
}

void StickyBall::Update(float deltaTime)
{

	//curr_rotation = 0.0f;
	radiusGrow = max(0.0, radiusGrow - deltaTime * radiusSlow);
	radius += radiusGrow * deltaTime;

	position.y = radius;
	position = position + deltaTime * velocity * GetMoveDir();
	currentSpin += spinSpeed * deltaTime;



	//Matrix spinTransform = Matrix::CreateFromYawPitchRoll(0.0f, currentSpin, 0.0f);
	//Matrix rotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f);
	//worldMat = Matrix::CreateScale(radius) * spinTransform * rotTransform * Matrix::CreateTranslation(position);
	Matrix allRotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, currentSpin, XM_PIDIV2);
	worldMat = Matrix::CreateScale(radius) * allRotTransform * Matrix::CreateTranslation(position);

	//std::cout << "( " << position.x << ", " << position.y << ", " << position.z << " )\n";


	//Matrix viewMat = camera->GetViewMatrix();
	//Matrix projMat = camera->GetProjectionMatrix();

	//cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}

void StickyBall::UpdateScale()
{
	worldMat = Matrix::CreateScale(radius)
		* Matrix::CreateFromYawPitchRoll(currentRotation, currentSpin, XM_PIDIV2)
		* Matrix::CreateTranslation(position);
}

void StickyBall::Grow(float volume)
{
	radiusGrow = 0.5f / sqrt(radius);
}
