#pragma once

#include <d3d11.h>
#include "Renderer.h"

class Bindable
{
public:
	virtual void Bind() = 0;
	virtual ~Bindable() = default;

protected:
	static ID3D11DeviceContext* GetContext(Renderer& renderer);
	static ID3D11Device* GetDevice(Renderer& renderer);
};