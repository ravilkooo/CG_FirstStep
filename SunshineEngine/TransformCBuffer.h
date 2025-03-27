#pragma once
#include <directxmath.h>
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

namespace Bind
{
	class TransformCBuffer : public Bindable
	{
	protected:
		struct Transforms
		{
			//DirectX::XMMATRIX model;
			//DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCBuffer(ID3D11Device* device, const Drawable* parent);
		void Bind(ID3D11DeviceContext* context) noexcept override;
	private:
		VertexConstantBuffer<Transforms> vcbuf;
		const Drawable* pParent = nullptr;
	};
}
