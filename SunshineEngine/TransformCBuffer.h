#pragma once
#include <directxmath.h>
#include "Bindable.h"
#include "SceneNode.h"

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
		TransformCBuffer(ID3D11Device* device, const SceneNode* parent);
		void Bind(ID3D11DeviceContext* context) noexcept override;
		//void InitializeParentReference(const Drawable& parent) noexcept override;
		//std::unique_ptr<CloningBindable> Clone() const noexcept override;
	//protected:
		//void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		//Transforms GetTransforms(Graphics& gfx) noxnd;
	private:
		VertexConstantBuffer<Transforms> vcbuf;
		const SceneNode* pParent = nullptr;
	};

}
