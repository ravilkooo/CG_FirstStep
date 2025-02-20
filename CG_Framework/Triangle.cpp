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

}

void Triangle::Draw()
{
}
