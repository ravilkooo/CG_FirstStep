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


	CommonVertex _points[6] = {
		CommonVertex(DirectX::XMFLOAT3(0.75f, 0.25f, 0.25f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(0.25f, -0.25f, 0.25f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)),
		CommonVertex(DirectX::XMFLOAT3(0.75f, -0.25f, 0.25f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f))
	};
	vertices = (CommonVertex*)malloc(3 * sizeof(CommonVertex));
	verticesNum = 3;

	for (int i = 0; i < 6; i++)
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

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Triangle::Triangle(CommonVertex* vertices)
{
	int _ind[3] = { 0, 1, 2 };
	indices = (int*)malloc(3 * sizeof(int));
	indicesNum = 3;
	for (int i = 0; i < 3; i++)
	{
		indices[i] = _ind[i];
	}

	this->vertices = (CommonVertex*)malloc(3 * sizeof(CommonVertex));
	verticesNum = 3;

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

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

void Triangle::Update(float deltaTime)
{
	DirectX::XMMATRIX rotZMat = DirectX::XMMatrixTranslation(0.03f * deltaTime, 0.f, 0.0f);
	cb.wvpMat = cb.wvpMat * rotZMat;
}
