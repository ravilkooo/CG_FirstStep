#include "LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera)
	:
	RenderPass("LightPass", device, context)
{
	this->pGBuffer = pGBuffer;
	this->camera = camera;

	D3D11_TEXTURE2D_DESC lightDesc = {};
	lightDesc.Width = screenWidth;
	lightDesc.Height = screenHeight;
	lightDesc.MipLevels = 1;
	lightDesc.ArraySize = 1;
	lightDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	lightDesc.SampleDesc.Count = 1;
	lightDesc.Usage = D3D11_USAGE_DEFAULT;
	lightDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&lightDesc, nullptr, pGBuffer->pLightBuffer.GetAddressOf());
	// Normal SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
	descSRV.Format = lightDesc.Format;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2DArray.MostDetailedMip = 0;
	descSRV.Texture2DArray.MipLevels = 1;
	descSRV.Texture2DArray.FirstArraySlice = 0;
	descSRV.Texture2DArray.ArraySize = 1;
	device->CreateShaderResourceView(pGBuffer->pLightBuffer.Get(), &descSRV, pGBuffer->pLightSRV.GetAddressOf());
	// Normal RTV
	HRESULT hr = device->CreateRenderTargetView(pGBuffer->pLightBuffer.Get(), nullptr, pGBuffer->pLightRTV.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	gBufferRTV = pGBuffer->pLightRTV.Get();

	// Viewport
	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	camPCB = new Bind::PixelConstantBuffer<CamPCB>(device,
		{ XMMatrixIdentity(), XMMatrixIdentity(),
		(XMFLOAT3)camera->GetPosition(), 0 },
		0u);
	AddPerFrameBind(camPCB);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	AddPerFrameBind(new Bind::BlendState(device, blendDesc));

	// Particle test
	{
		particles.resize(numParticles);
		for (size_t i = 0; i < numParticles; i++)
		{

			particles[i] = {
				{3 * cosf(i * XM_2PI * XM_2PI / numParticles), 3 * sinf(i * XM_2PI / numParticles), 0},
				0.05,
				{
					abs(cosf(i * XM_2PI / numParticles)),
					abs(cosf(i * XM_2PI / numParticles + 1)),
					abs(cosf(i * XM_2PI / numParticles + 2)),
					1
				},
			};
		}

		D3D11_BUFFER_DESC particleBufDesc = {};
		particleBufDesc.ByteWidth = sizeof(Particle) * numParticles;
		particleBufDesc.Usage = D3D11_USAGE_DYNAMIC;
		particleBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		particleBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		particleBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		particleBufDesc.StructureByteStride = sizeof(Particle);

		// D3D11_UNORDERED_ACCESS_VIEW_DESC

		/*
		D3D11_SUBRESOURCE_DATA initDataParticles = {};
		initDataParticles.pSysMem = &particles;
		*/
		GetDevice()->CreateBuffer(&particleBufDesc, nullptr, &particleBuffer);

		D3D11_MAPPED_SUBRESOURCE mapped;
		GetDeviceContext()->Map(particleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, particles.data(), sizeof(Particle)* numParticles);
		GetDeviceContext()->Unmap(particleBuffer, 0);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Buffer.NumElements = numParticles;

		GetDevice()->CreateShaderResourceView(particleBuffer, &srvDesc, &particleSRV);


		struct DrawArgs {
			UINT IndexCountPerInstance;
			UINT InstanceCount;
			UINT StartIndexLocation;
			INT BaseVertexLocation;
			UINT StartInstanceLocation;
		};

		DrawArgs args = {
			6, // Кол-во индексов на частицу (квад)
			numParticles, // Кол-во частиц
			0, 0, 0
		};

		// Создаем буфер для аргументов
		D3D11_BUFFER_DESC argsDesc = {};
		argsDesc.ByteWidth = sizeof(DrawArgs);
		argsDesc.Usage = D3D11_USAGE_DEFAULT;
		argsDesc.BindFlags = 0;
		argsDesc.CPUAccessFlags = 0;
		argsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;


		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &args;

		GetDevice()->CreateBuffer(&argsDesc, &initData, &indirectArgsBuffer);

		int indices[6] = {0, 1, 2, 3, 2, 1};
		ps_ib = new Bind::IndexBuffer(GetDevice(), indices, 6);
		ps_vs = new Bind::VertexShader(GetDevice(), L"./Shaders/ParticleVS.hlsl");
		ps_ps = new Bind::PixelShader(GetDevice(), L"./Shaders/ParticleVS.hlsl");
		auto vs_blob = ps_vs->GetBytecode();
		
		D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr;
		ps_ia = new Bind::InputLayout(GetDevice(), IALayoutInputElements, 0, vs_blob);
		
		ps_vcb = new Bind::VertexConstantBuffer<XMMATRIX>(GetDevice(), XMMatrixIdentity(), 0u);
	}

}

void LightPass::StartFrame()
{
	context->OMSetRenderTargets(1, &gBufferRTV, pGBuffer->pDepthDSV.Get());
	float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(gBufferRTV, colorBlack);
	//context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);

	XMFLOAT3 camPos = camera->GetPosition();
	XMMATRIX vMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetViewMatrix()));
	XMMATRIX pMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetProjectionMatrix()));
	// camera->GetProjectionMatrix()
	camPCB->Update(GetDeviceContext(), { vMatInverse, pMatInverse, camPos, 0 });
}

void LightPass::Pass(const Scene& scene)
{
	BindAllPerFrame();

	for (SceneNode* node : scene.nodes) {
		if (!node->HasTechnique(techniqueTag))
			continue;
		// Turn off depth write
		// Prepare Lights (depthstencil desc and rast desc) and bind (depthstencil and rast)
		LightObject* lObj = dynamic_cast<LightObject*>(node);
		if (lObj) {
			lObj->UpdateBuffers(context);
			LightObject::LightPosition lightPos = lObj->GetLightPositionInFrustum(GetCamera());

			/*
			if (dynamic_cast<PointLight*>(node)) {
				if (lightPos == LightObject::LightPosition::BEHIND_NEAR_PLANE
					|| lightPos == LightObject::LightPosition::INSIDE
					|| lightPos == LightObject::LightPosition::INTERSECT_FAR_PLANE
					|| lightPos == LightObject::LightPosition::FILL)
				*/
			auto dsDesc = lObj->GetDepthStencilDesc(lightPos);
			auto rastDesc = lObj->GetRasterizerDesc(lightPos);

			ID3D11RasterizerState* rasterState;
			ID3D11DepthStencilState* depthState;
			device->CreateRasterizerState(&rastDesc, &rasterState);
			device->CreateDepthStencilState(&dsDesc, &depthState);

			context->OMSetDepthStencilState(depthState, 0);
			context->RSSetState(rasterState);

			node->PassTechnique(techniqueTag, GetDeviceContext());

			context->OMSetDepthStencilState(nullptr, 0);
			context->RSSetState(nullptr);

		}
		else {
			continue;
		}
	}

	// Particle test
	{
		currTime += 0.08;
		for (size_t i = 0; i < numParticles; i++)
		{
			particles[i].Position = { 3 * cosf((i + currTime) * XM_2PI * 6 / numParticles + 0.8), 3 * sinf((i + currTime) * XM_2PI / numParticles), 0 };
			particles[i].Size = 0.05 + 0.03 * sinf(i * 0.17 + currTime);
		}
		D3D11_MAPPED_SUBRESOURCE mapped;
		GetDeviceContext()->Map(particleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, particles.data(), sizeof(Particle) * numParticles);
		GetDeviceContext()->Unmap(particleBuffer, 0);


		ps_ib->Bind(GetDeviceContext());
		ps_vcb->Update(GetDeviceContext(), Matrix::Identity * camera->GetViewMatrix() * camera->GetProjectionMatrix());
		ps_vcb->Bind(GetDeviceContext());
		ps_ia->Bind(GetDeviceContext());
		ps_vs->Bind(GetDeviceContext());
		ps_ps->Bind(GetDeviceContext());

		// Привязываем буфер частиц как SRV
		ID3D11ShaderResourceView* srvs[] = { particleSRV };
		GetDeviceContext()->VSSetShaderResources(5, 1, srvs);

		// Устанавливаем шейдеры, InputLayout и топологию
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* rasterState;
		ID3D11DepthStencilState* depthState;
		device->CreateRasterizerState(&rasterDesc, &rasterState);
		device->CreateDepthStencilState(&dsDesc, &depthState);

		context->OMSetDepthStencilState(depthState, 0);
		context->RSSetState(rasterState);
		
		GetDeviceContext()->DrawIndexedInstancedIndirect(
			indirectArgsBuffer, // Буфер с аргументами
			0                  // Смещение в буфере
		);
	}
}

void LightPass::EndFrame()
{
	//ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };

	ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);
	//context->PSSetShaderResources(0, NULL, NULL);
	context->OMSetRenderTargets(0, NULL, NULL);
}

Camera* LightPass::GetCamera()
{
	return camera;
}

void LightPass::SetCamera(Camera* camera)
{
	this->camera = camera;
}
