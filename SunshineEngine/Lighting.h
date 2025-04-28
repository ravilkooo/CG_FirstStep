#pragma once

#include <d3d11.h>
#include <directxmath.h>

struct DirectionalLight_old
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Direction;
	float pad;
};

struct PointLight_old
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Att;
	float pad;
};

struct SpotLight_old
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Direction;
	float Spot;

	DirectX::XMFLOAT3 Att;
	float pad;
};

struct LightData_old {
	DirectionalLight_old dLight;
	PointLight_old pointLights[14];
	SpotLight_old spotLight;
};

/*
cbuffer LightBuffer : register(b0) // per frame
{
	DirectionalLight dLight;
	PointLight pointLights[6];
};*/