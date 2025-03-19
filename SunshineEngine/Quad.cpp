#include "Quad.h"

Quad::Quad()
{

	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int *) malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	CommonVertex _points[4] = {
		CommonVertex(DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(-0.25f, -0.25f, 0.25f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(0.25f, -0.25f, 0.25f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(-0.25f, 0.25f, 0.25f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
	};
	vertices = (CommonVertex*)malloc(4 * sizeof(CommonVertex));
	verticesNum = 4;
	
	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _points[i];
	}

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(2 * sizeof(D3D11_INPUT_ELEMENT_DESC));
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

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Quad::Quad(DirectX::XMFLOAT4 position, float width)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}

	width = width > 0 ? width: 0.01;
	CommonVertex _points[8] = {
		CommonVertex(DirectX::XMFLOAT3(position.x + width, position.y + width, position.z + 0.0),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x, position.y, position.z + 0.0),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x + width,position.y, position.z + 0.0),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x, position.y + width, position.z + 0.0),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
	};
	vertices = (CommonVertex*)malloc(4 * sizeof(CommonVertex));
	verticesNum = 4;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _points[i];
	}

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(2 * sizeof(D3D11_INPUT_ELEMENT_DESC));
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

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Quad::Quad(DirectX::XMFLOAT4 position, float width, float height)
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
	CommonVertex _points[8] = {
		CommonVertex(DirectX::XMFLOAT3(position.x + width, position.y + height, position.z + 0.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x, position.y, position.z + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x + width, position.y, position.z + 0.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(position.x, position.y + height, position.z + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
	};
	vertices = (CommonVertex*)malloc(4 * sizeof(CommonVertex));
	verticesNum = 4;

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = _points[i];
	}

	IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(2 * sizeof(D3D11_INPUT_ELEMENT_DESC));
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

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

void Quad::Update(float deltaTime)
{
	DirectX::XMMATRIX rotZMat = DirectX::XMMatrixRotationZ(0.3f * deltaTime);
	cb.wvpMat = cb.wvpMat * rotZMat;

	//std::cout << "H\n";
}
