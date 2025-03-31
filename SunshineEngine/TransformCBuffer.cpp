#include "TransformCBuffer.h"
#include <iostream>

namespace Bind {

	TransformCBuffer::TransformCBuffer(ID3D11Device* device, const Drawable* parent, UINT slot)
		: vcbuf(device, slot), pParent(parent)
	{
	}

	void TransformCBuffer::Bind(ID3D11DeviceContext* context) noexcept
	{
		const auto wMat = pParent->worldMat;
		const auto vpMat = pParent->GetViewMatrix() * pParent->GetProjectionMatrix();
		DirectX::XMMATRIX A = wMat;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR det = XMMatrixDeterminant(A);
		const auto wMatInvTranspose = DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));

		const Transforms tf = {
				wMat, wMatInvTranspose, vpMat
		};
		vcbuf.Update(context, tf);
		vcbuf.Bind(context);
	}
}
