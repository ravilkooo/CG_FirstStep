#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Sampler
{
public:
	Sampler(ID3D11Device* device);
	void Bind(ID3D11DeviceContext* context);

protected:
	ID3D11SamplerState* pSampler;

};
