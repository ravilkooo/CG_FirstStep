#include "DL_ShadowMapPass.h"
#include <iostream>

DL_ShadowMapPass::DL_ShadowMapPass(ID3D11Device* device, ID3D11DeviceContext* context,
	UINT mapWidth, UINT mapHeight, DirectionalLight_old dlight)
	:
	RenderPass("DL_ShadowMapPass", device, context)
{
	this->mapWidth = mapWidth;
	this->mapHeight = mapHeight;

	this->dlight = dlight;

	lightPos = { 0, 30, -80 };
	lightViewCamera = new Camera(mapWidth / mapHeight);
	lightViewCamera->SetPosition(lightPos);
	lightViewCamera->SetTarget(lightPos + dlight.Direction);
	Vector3 cameraUp = { 0, 1, 0 };
	cameraUp.Normalize();
	lightViewCamera->SetUp(cameraUp);
	lightViewCamera->SwitchProjection();
	lightViewCamera->SetViewWidth(100.0f);
	lightViewCamera->SetViewHeight(100.0f);
	
	lightViewCamera->SetNearZ(cascadeBounds[2]);
	lightViewCamera->SetFarZ(cascadeBounds[3]);
	

	// Viewport for rendering z-buffer from light
	smViewport.TopLeftX = 0.0f;
	smViewport.TopLeftY = 0.0f;
	smViewport.Width = static_cast<float>(mapWidth);
	smViewport.Height = static_cast<float>(mapHeight);
	smViewport.MinDepth = 0.0f;
	smViewport.MaxDepth = 1.0f;

	// Texture for depth values (for shadowing)
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = mapWidth;
	depthDesc.Height = mapHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 4;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthDesc, nullptr, &shadowTexture);

	// View texture as Depth buffer while rendering from light camera
	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = { };
	dViewDesc.Flags = 0;
	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dViewDesc.Texture2DArray.MipSlice = 0;
	dViewDesc.Texture2DArray.ArraySize = 1;

	for (size_t i = 0; i < 4; i++)
	{
		dViewDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
		device->CreateDepthStencilView(shadowTexture, &dViewDesc, &(depthDSV[i]));
	}

	Bind::VertexShader* vertexShader = new Bind::VertexShader(
		device, L"./Shaders/ShadowMapVShader.hlsl");
	AddPerFrameBind(vertexShader);

	{
		D3D11_INPUT_ELEMENT_DESC shadowIALayoutInputElements[1] = {
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 },
		};

		AddPerFrameBind(
			new Bind::InputLayout(device, shadowIALayoutInputElements, 1, vertexShader->GetBytecode())
		);
	}

	// Buffer for transformation respect to light camera

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2 (for XY; Z stays the same)
	DirectX::XMMATRIX T = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	};

	
	for (size_t i = 0; i < 4; i++)
	{
		lightViewCamera->SetNearZ(cascadeBounds[i] - (i > 0 ? frustumBias : 0));
		lightViewCamera->SetFarZ(cascadeBounds[i + 1]);

		cascadesData.cascades[i] = {
			lightViewCamera->GetViewMatrix(),
			lightViewCamera->GetProjectionMatrix(),
			DirectX::XMMatrixIdentity()
		};

		cascadesData.cascades[i].shadowTransformFull =
			cascadesData.cascades[i].lightView
			* cascadesData.cascades[i].lightProjection
			* T;

	}
	cascadesData.distances.x = cascadeBounds[1];
	cascadesData.distances.y = cascadeBounds[2];
	cascadesData.distances.z = cascadeBounds[3];
	cascadesData.distances.w = cascadeBounds[4];
	
	shadowTransformsConstantBuffer = new Bind::VertexConstantBuffer<ShadowTransformData>(
		device, cascadesData.cascades[0], 0u);
	AddPerFrameBind(shadowTransformsConstantBuffer);

	// Rasterizer for depth bias to get rid of self-shadowing
	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias = 1000000;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 2.0f;

	AddPerFrameBind(new Bind::Rasterizer(device, rastDesc));

	cascadesConstantBuffer = new Bind::PixelConstantBuffer<DL_ShadowMapPass::CascadesData>(device, cascadesData, 1u);
}


DL_ShadowMapPass::ShadowTransformData DL_ShadowMapPass::GenerateBoundingFrustum(UINT cascadeNum)
{
	// дл€ каждого фруструма у камеры мен€ютс€ следующие настройи:
	// nearestZ - у всех одинаковый (например 0.01)
	// SetViewWidth - вычисл€етс€ по значени€м точек (находим right вектор как вект произв Up X Direction)
	// SetViewHeight - вычисл€етс€ по значени€м точек
	// SetPosition - выбираем на уровне вершины фрустума, котора€ располагаетс€ "выше" (в противоположную сторону -) всех остальных вдоль оси направлени€ света, затем отмер€ем назад значение nearZ
	// и выбираем ровно посередине исход€ из значений исполььзованных при расчЄте SetViewWidth и SetViewHeight
	// farestZ - макс рассто€ние от SetPosition до вершин фрустума вдоль направление света
	 
	// ѕолучаем крайние точки подфрустумов игрока
	playerCamera->SetNearZ(cascadeBounds[cascadeNum] - (cascadeNum > 0 ? frustumBias : 0));
	playerCamera->SetFarZ(cascadeBounds[cascadeNum + 1] + frustumBias);

	Matrix playerViewMat = playerCamera->GetViewMatrix();
	Matrix playerProjMat = playerCamera->GetProjectionMatrix();

	Matrix viewInverse = playerViewMat.Invert();
	Matrix projInverse = playerProjMat.Invert();

	Vector3 farLeftPoint, farRightPoint, highestPoint, lowestPoint, nearZPoint, farZPoint;
	float farLeft, farRight, highest, lowest, nearestZ, farestZ;
	{
		Vector4 _v = { 0, 0, 0, 1 };
		_v = Vector4::Transform(_v, projInverse);
		_v = Vector4::Transform(_v, viewInverse);
		_v = _v / _v.w;
		Vector3 _w = Vector3(_v);
		farLeftPoint = _w; farRightPoint = _w; highestPoint = _w; lowestPoint = _w; nearZPoint = _w; farZPoint = _w;
	}
	
	Vector3 zDir = Vector3(this->dlight.Direction);
	zDir.Normalize();
	Vector3 camUpDir = lightViewCamera->GetUp();
	camUpDir.Normalize();
	Vector3 xDir = camUpDir.Cross(zDir);
	xDir.Normalize();
	Vector3 yDir = zDir.Cross(xDir);
	yDir.Normalize();

	for (float i = -1; i < 2; i += 2)
	{
		for (float j = -1; j < 2; j +=2)
		{
			for (float k = 0; k < 2; k++)
			{
				Vector4 _v = { i, j, k, 1 };
				_v = Vector4::Transform(_v, projInverse);
				_v = Vector4::Transform(_v, viewInverse);
				_v = _v / _v.w;
				if (farRightPoint.Dot(xDir) < Vector3(_v).Dot(xDir)) { farRightPoint = Vector3(_v); }
				else
				if (farLeftPoint.Dot(xDir) > Vector3(_v).Dot(xDir)) { farLeftPoint = Vector3(_v); }

				if (highestPoint.Dot(yDir) < Vector3(_v).Dot(yDir)) { highestPoint = Vector3(_v); }
				else
				if (lowestPoint.Dot(yDir) > Vector3(_v).Dot(yDir)) { lowestPoint = Vector3(_v); }

				if (farZPoint.Dot(zDir) < Vector3(_v).Dot(zDir)) { farZPoint = Vector3(_v); }
				else
				if (nearZPoint.Dot(zDir) > Vector3(_v).Dot(zDir)) { nearZPoint = Vector3(_v); }


				//std::cout << _v.x << ",\t" << _v.y << ",\t" << _v.z << "\n";
			}
		}
	}
	farRight = farRightPoint.Dot(xDir);
	farLeft = farLeftPoint.Dot(xDir);
	highest = highestPoint.Dot(yDir);
	lowest = lowestPoint.Dot(yDir);
	nearestZ = nearZPoint.Dot(zDir);
	farestZ = farZPoint.Dot(zDir);

	float _nearZ = 0.01;

	Vector3 newCamPos =
		(farRight + farLeft) * 0.5 * xDir
		+ (highest + lowest) * 0.5 * yDir
		+ (nearestZ - _nearZ) * zDir; // -0.01 так как камера немного отдалена от nearZ
	

	lightViewCamera->SetPosition(newCamPos);
	lightViewCamera->SetTarget(newCamPos + dlight.Direction);
	lightViewCamera->SetNearZ(_nearZ);
	lightViewCamera->SetFarZ(farestZ - nearestZ + _nearZ);
	lightViewCamera->SetViewWidth(farRight - farLeft);
	lightViewCamera->SetViewHeight(highest - lowest);

	{
		DirectX::XMMATRIX T = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		};

		cascadesData.cascades[cascadeNum] = {
			lightViewCamera->GetViewMatrix(),
			lightViewCamera->GetProjectionMatrix(),
			DirectX::XMMatrixIdentity()
		};

		cascadesData.cascades[cascadeNum].shadowTransformFull =
			cascadesData.cascades[cascadeNum].lightView
			* cascadesData.cascades[cascadeNum].lightProjection
			* T;
	}

	return cascadesData.cascades[cascadeNum];
}

void DL_ShadowMapPass::MapCurrentCascadeData()
{
	shadowTransformsConstantBuffer->Update(GetDeviceContext(), cascadesData.cascades[currCascade]);
}

void DL_ShadowMapPass::StartFrame()
{

}

void DL_ShadowMapPass::Pass(const Scene& scene)
{

	for (currCascade = 0; currCascade < 4; currCascade++)
	{
		context->OMSetRenderTargets(0, NULL, NULL);
		context->PSSetShaderResources(0u, 0u, nullptr);
		context->RSSetViewports(1, &smViewport);

		// Set null render target because we are only going to draw
		// to depth buffer. Setting a null render target will disable
		// color writes.
		ID3D11RenderTargetView* rtvDepth[1] = { 0 };

		context->OMSetRenderTargets(1, rtvDepth, depthDSV[currCascade]);
		context->ClearDepthStencilView(depthDSV[currCascade], D3D11_CLEAR_DEPTH, 1.0f, 0);

		// Get data for current cascade
		GenerateBoundingFrustum(currCascade);
		MapCurrentCascadeData();

		// --- Per Frame ---
		context->PSSetShader(nullptr, nullptr, 0u);

		BindAllPerFrame();

		for (SceneNode* node : scene.nodes) {
			if (!node->HasTechnique(techniqueTag))
				continue;
			node->PassTechnique(techniqueTag, GetDeviceContext());
		}
		
	}
}

void DL_ShadowMapPass::EndFrame()
{
	cascadesConstantBuffer->Update(context.Get(), cascadesData);
	context->OMSetRenderTargets(0, NULL, NULL);
}

ID3D11Texture2D* DL_ShadowMapPass::GetTexture()
{
	return shadowTexture;
}

Camera* DL_ShadowMapPass::GetFrustumCamera()
{
	return lightViewCamera;
}

void DL_ShadowMapPass::GetFrustumBoundsZ(int i, float* nearZ, float* farZ)
{
	*nearZ = cascadeBounds[i];
	*farZ = cascadeBounds[i + 1];
}

