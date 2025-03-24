#pragma once

#include <d3d11.h>
//#include "SceneNode.h"
//#include "Renderer.h"
namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) noexcept = 0;
		virtual ~Bindable() = default;

	protected:
		//static ID3D11DeviceContext* GetContext(Renderer& renderer);
		//static ID3D11Device* GetDevice(Renderer& renderer);
	};
}