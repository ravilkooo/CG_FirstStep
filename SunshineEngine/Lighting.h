#pragma once

#include <d3d11.h>
#include <directxmath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Direction;
	float pad;
};

struct PointLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Att;
	float pad;
};

struct SpotLight
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

struct LightData {
	DirectionalLight dLight;
	PointLight pointLights[14];
	SpotLight spotLight;
};

/*
cbuffer LightBuffer : register(b0) // per frame
{
	DirectionalLight dLight;
	PointLight pointLights[6];
};*/