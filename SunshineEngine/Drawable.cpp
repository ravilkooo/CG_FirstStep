#include "Drawable.h"

void Drawable::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept {
	context->DrawIndexed(indicesNum, 0, 0);
};
