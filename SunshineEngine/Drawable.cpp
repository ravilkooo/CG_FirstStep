#include "Drawable.h"
#include "Bindable.h"

using namespace Bind;

void Drawable::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept {
	context->DrawIndexed(indicesNum, 0, 0);
};

void Drawable::AddBind(Bind::Bindable* bind)
{
	bindables.push_back(bind);
}

void Drawable::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{

	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}
	for (size_t i = 0; i < GetStaticBinds().size(); i++)
	{
		GetStaticBinds()[i]->Bind(context.Get());
	}
}

DirectX::XMMATRIX Drawable::GetViewMatrix() const
{
	return camera->GetViewMatrix();
}

DirectX::XMMATRIX Drawable::GetProjectionMatrix() const
{
	return camera->GetProjectionMatrix();
}

DirectX::XMFLOAT3 Drawable::GetCameraPosition() const
{
	return camera->GetPosition();
}
