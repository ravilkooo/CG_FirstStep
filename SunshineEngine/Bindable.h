#pragma once

#include <d3d11.h>
#include <wrl.h>
//#include "SceneNode.h"
//#include "Renderer.h"
namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind(ID3D11DeviceContext* context) noexcept = 0;
		//virtual void Update();
		virtual ~Bindable() = default;
	};
}