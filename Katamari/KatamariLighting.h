#include <d3d11.h>
#include <directxmath.h>

struct LightData {
	struct DirectionalLight
	{
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;

		DirectX::XMFLOAT3 Direction;
		float pad;
	} dLight;

	struct PointLight
	{
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT3 Position;
		float Range;

		DirectX::XMFLOAT3 Att;
		float pad;
	} pointLights[6];
};


/*
cbuffer LightBuffer : register(b0) // per frame
{
	DirectionalLight dLight;
	PointLight pointLights[6];
};*/