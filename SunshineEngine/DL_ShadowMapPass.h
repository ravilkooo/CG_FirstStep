#pragma once

#include "RenderPass.h"
#include "BindableCollection.h"
#include "Lighting.h"

class DL_ShadowMapPass :
    public RenderPass
{
public:
    DL_ShadowMapPass(ID3D11Device* device, ID3D11DeviceContext* context,
        UINT mapWidth, UINT mapHeight, DirectionalLight dlight);

	ID3D11Texture2D* GetTexture();
	Camera* GetFrustumCamera();
	void GetFrustumBoundsZ(int i, float* nearZ, float* farZ);

	struct ShadowTransformData
	{
		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;
		DirectX::XMMATRIX shadowTransformFull;
	};
	struct CascadesData {
		ShadowTransformData cascades[4];
		DirectX::XMFLOAT4 distances;
	} cascadesData;
private:
	Camera* lightViewCamera;
	UINT mapWidth = 512;
	UINT mapHeight = 512;
	Vector3 lightPos;
	ID3D11Texture2D* shadowTexture;
	ID3D11DepthStencilView* depthDSV[4];
	D3D11_VIEWPORT smViewport;

	UINT currCascade = 0;

	Bind::VertexConstantBuffer<ShadowTransformData>* shadowTransformsConstantBuffer;
	void MapCurrentCascadeData();

	//float cascadeBounds[5] = { 0.1f, 10.0f, 30.0f, 10.0f, 40.0f };
	float cascadeBounds[5] = { 0.1f, 10.0f, 30.0f, 80.0f, 200.0f };
	float FrustumBias = 10.0f;

	void StartFrame() override;
	void EndFrame() override;
	void Pass(const Scene& scene) override;



};

