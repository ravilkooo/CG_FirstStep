#include "Sampler.h"
namespace Bind
{
	Sampler::Sampler(ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc, UINT slot)
		: slot(slot)
	{
		device->CreateSamplerState(&samplerDesc, &pSampler);
	}
	void Sampler::Bind(ID3D11DeviceContext* context) noexcept
	{
		context->PSSetSamplers(slot, 1u, pSampler.GetAddressOf());
	}

}
