#include "TransformCBuffer.h"

namespace Bind {

	TransformCBuffer::TransformCBuffer(ID3D11Device* device, const Drawable* parent, UINT slot)
		: vcbuf(device, slot), pParent(parent)
	{
	}

	void TransformCBuffer::Bind(ID3D11DeviceContext* context) noexcept
	{
		const auto wMat = pParent->worldMat;
		const auto vpMat = pParent->GetViewMatrix() * pParent->GetProjectionMatrix();
		const Transforms tf = {
				wMat, vpMat
		};
		vcbuf.Update(context, tf);
		vcbuf.Bind(context);
	}
}
