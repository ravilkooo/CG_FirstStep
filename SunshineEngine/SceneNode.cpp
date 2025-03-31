#include "SceneNode.h"

SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}


void SceneNode::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{

	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}
}

void SceneNode::AddBind(Bind::Bindable* bind)
{
	bindables.push_back(bind);
}

DirectX::XMMATRIX SceneNode::GetViewMatrix() const
{
	return camera->GetViewMatrix();
}

DirectX::XMMATRIX SceneNode::GetProjectionMatrix() const
{
	return camera->GetProjectionMatrix();
}

DirectX::XMFLOAT3 SceneNode::GetCameraPosition() const
{
	return camera->GetPosition();
}
