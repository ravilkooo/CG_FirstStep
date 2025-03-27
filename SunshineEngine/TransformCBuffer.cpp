#include "TransformCBuffer.h"

namespace Bind {

	TransformCBuffer::TransformCBuffer(ID3D11Device* device, const SceneNode* parent)
		: vcbuf(device), pParent(parent)
	{
	}

	void TransformCBuffer::Bind(ID3D11DeviceContext* context) noexcept
	{
		vcbuf.Update(context, 
			{
				pParent->worldMat
				* pParent->camera->GetViewMatrix()
				* pParent->camera->GetProjectionMatrix()
			});
		vcbuf.Bind(context);
	}
}
