#include "Floor.h"
#include <ShapeGenerator.h>

Floor::Floor()
{

}

Floor::Floor(ID3D11Device* device)
{
	this->device = device;
	UINT sliceCount = 10;
	UINT elevationCount = 5;
	XMFLOAT4 col(0.9f, 0.0f, 0.0f, 1.0f);
	CreateRandomHeightPlane(50.0f, 50.0f, 50, 50, 0.2f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), &vertices, &verticesNum, &indices, &indicesNum);
	
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

	shaderFilePath = L"./Shaders/FloorShader.hlsl";
	this->textures.push_back(Texture(device, "models\\Textures\\carpet.dds", aiTextureType_DIFFUSE));	
	hasTexture = true;
}

void Floor::Update(float deltaTime)
{
	//Matrix viewMat = camera->GetViewMatrix();
	//Matrix projMat = camera->GetProjectionMatrix();


	//cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}

Vector3 Floor::GetCenterLocation()
{
	return Vector3::Zero;
}
