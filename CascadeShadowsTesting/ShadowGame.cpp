#include "ShadowGame.h"

ShadowGame::ShadowGame()
{
	applicationName = L"ShadowGame";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();

	scene = Scene();

	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);
	//renderer.camera.SetPosition({ 0, 0, -1 });
	renderer.camera.SetPosition(lightPos);

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &ShadowGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &ShadowGame::HandleMouseMove);

	// light stuff

	lightData.pointLight.Ambient = { 0.2f, 0.2f, 0.2f, 1 };
	lightData.pointLight.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData.pointLight.Specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData.pointLight.Position = lightPos;
	lightData.pointLight.Range = 100.0f;
	lightData.pointLight.Att = { 0.01f, 1.0f, 0.0f };


	light_pcb = new Bind::PixelConstantBuffer<LightData>(renderer.GetDevice(), lightData, 0u);
	renderer.AddPerFrameBind(light_pcb);

	cam_pcb = new Bind::PixelConstantBuffer<Camera_PCB>(renderer.GetDevice(), { renderer.camera.GetPosition() }, 1u);
	renderer.AddPerFrameBind(cam_pcb);


	// shadow stuff
	
	// For CubeMap
	XMFLOAT3 ups[6] = {
	XMFLOAT3(0.0f, 1.0f, 0.0f), // +X
	XMFLOAT3(0.0f, 1.0f, 0.0f), // -X
	XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
	XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
	XMFLOAT3(0.0f, 1.0f, 0.0f), // +Z
	XMFLOAT3(0.0f, 1.0f, 0.0f) // -Z
	};

	XMFLOAT3 targets[6] = {
	XMFLOAT3(lightPos.x + 1.0f, lightPos.y, lightPos.z), // +X
	XMFLOAT3(lightPos.x - 1.0f, lightPos.y, lightPos.z), // -X
	XMFLOAT3(lightPos.x, lightPos.y + 1.0f, lightPos.z), // +Y
	XMFLOAT3(lightPos.x, lightPos.y - 1.0f, lightPos.z), // -Y
	XMFLOAT3(lightPos.x, lightPos.y, lightPos.z + 1.0f), // +Z
	XMFLOAT3(lightPos.x, lightPos.y, lightPos.z - 1.0f) // -Z 
	};
	/*
	for (size_t i = 0; i < 6; i++) {
		lightViewCameras[i] = new Camera(smSizeX / smSizeY);
		lightViewCameras[i]->SetPosition(lightPos);
		lightViewCameras[i]->SetTarget(targets[i]);
		lightViewCameras[i]->SetUp(ups[i]);
	}
	*/

	lightViewCamera = new Camera(smSizeX / smSizeY);
	lightViewCamera->SetPosition(lightPos);
	lightViewCamera->SetTarget(targets[4]);
	lightViewCamera->SetUp(ups[4]);

	smViewport.TopLeftX = 0.0f;
	smViewport.TopLeftY = 0.0f;
	smViewport.Width = static_cast<float>(smSizeX);
	smViewport.Height = static_cast<float>(smSizeY);
	smViewport.MinDepth = 0.0f;
	smViewport.MaxDepth = 1.0f;


	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = smSizeX;
	depthDesc.Height = smSizeY;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	//depthDesc.CPUAccessFlags = 0;
	 depthDesc.MiscFlags = 0;
	// depthDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	renderer.GetDevice()->CreateTexture2D(&depthDesc, nullptr, &shadowTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = { };
	dViewDesc.Flags = 0;
	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dViewDesc.Texture2DArray.MipSlice = 0;
	dViewDesc.Texture2DArray.FirstArraySlice = 0;
	dViewDesc.Texture2DArray.ArraySize = 1;

	renderer.GetDevice()->CreateDepthStencilView(shadowTexture, &dViewDesc, &depthDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 1;

	renderer.GetDevice()->CreateShaderResourceView(shadowTexture, &srvDesc, &depthSRV);

	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias = 1000;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 1.0f;
	shadowRast = new Bind::Rasterizer(renderer.GetDevice(), rastDesc);

	
	renderer.AddPerFrameBind(new Bind::TextureB(renderer.GetDevice(), shadowTexture, depthSRV, 0u));

	TestCube* _tc = new TestCube(renderer.GetDevice(), 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 });
	_tc->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV4, XM_PIDIV4, 0));
	scene.AddNode(_tc);

	TestCube* _tc_2 = new TestCube(renderer.GetDevice(), 0.2f, 0.2f, 0.2f, { 0.5f, 0.0f, 0.0f }, { 0.9f, 0.7f, 0.5f, 1.0f });
	_tc_2->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.4, XM_PIDIV4 * 0.12, 0));
	scene.AddNode(_tc_2);

	TestCube* _tc_3 = new TestCube(renderer.GetDevice(), 10.0f, 10.0f, 1.0f, { 0.0f, 0.0f, 5.0f }, { 0.3f, 0.7f, 0.5f, 1.0f });
	scene.AddNode(_tc_3);


	for (auto node : scene.nodes)
	{
		node->camera = &(renderer.camera);
	}

	shadowableObjects.push_back(_tc);
	shadowableObjects.push_back(_tc_2);
	shadowableObjects.push_back(_tc_3);

	shadowVShader = new Bind::VertexShader(renderer.GetDevice(), L"./Shaders/ShadowMapVShader.hlsl");
	shadowInpLayout = new Bind::InputLayout(renderer.GetDevice(), _tc->IALayoutInputElements, _tc->numInputElements, shadowVShader->GetBytecode());
	// shadowVBuffer = new Bind::VertexBuffer(renderer.GetDevice(), _tc->vertices, _tc->verticesNum, sizeof(CommonVertex));
	shadowIBuffer = new Bind::IndexBuffer(renderer.GetDevice(), _tc->indices, _tc->indicesNum);
	{
		XMMATRIX LV = lightViewCamera->GetViewMatrix();
		XMMATRIX LP = lightViewCamera->GetProjectionMatrix();
		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2 (for XY; Z stays the same)
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);
		XMMATRIX SH = LV * LP * T;

		shadowTransforms = new Bind::VertexConstantBuffer<ShadowTransform>(renderer.GetDevice(),
			{ LV, LP, SH }, 1u);

		renderer.AddPerFrameBind(new Bind::PixelConstantBuffer<ShadowTransform>(renderer.GetDevice(),
			{ LV, LP, SH }, 2u));
	}

}

ShadowGame::~ShadowGame()
{
}

void ShadowGame::Run()
{
	MSG msg = {};
	bool isExitRequested = false;
	unsigned int frameCount = 0;
	float totalTime = 0;

	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}
		timer.Tick();
		deltaTime = timer.GetDeltaTime();
		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f) {
			float fps = frameCount * 1.0f / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(displayWindow.hWnd, text);

			frameCount = 0;
		}

		Update(deltaTime);
		Render();
	}
}


void ShadowGame::Update(float deltaTime)
{
	physEngine->Update(deltaTime);

	cam_pcb->Update(renderer.GetDeviceContext(), { renderer.camera.GetPosition() });

}


void ShadowGame::Render()
{
	BindDsvAndSetNullRenderTarget();
	DrawSceneToShadowMap();


	//renderer.GetDeviceContext()->PSSetShaderResources(0, 1u, &depthSRV);
	/*for (TestCube* node : shadowableObjects) {
		node->shadowPixelCBuffer->Bind(renderer.GetDeviceContext());
	}
	*/

	renderer.RenderScene(scene);
}



void ShadowGame::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		renderer.camera.MoveForward(deltaTime);
	}
	if (key == Keys::S)
	{
		renderer.camera.MoveBackward(deltaTime);
	}
	if (key == Keys::A)
	{
		renderer.camera.MoveLeft(deltaTime);
	}
	if (key == Keys::D)
	{
		renderer.camera.MoveRight(deltaTime);
	}
	if (key == Keys::Space)
	{
		renderer.camera.MoveUp(deltaTime);
	}

	if (key == Keys::LeftShift)
	{
		renderer.camera.MoveDown(deltaTime);
	}
}


void ShadowGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderer.camera.RotateYaw(deltaTime * args.Offset.x * 0.1);
	renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.1);
}

void ShadowGame::BindDsvAndSetNullRenderTarget()
{
	renderer.GetDeviceContext()->RSSetViewports(1, &smViewport);
	// Set null render target because we are only going to draw
	// to depth buffer. Setting a null render target will disable
	// color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	renderer.GetDeviceContext()->OMSetRenderTargets(1, renderTargets, depthDSV);
	renderer.GetDeviceContext()->ClearDepthStencilView(depthDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowGame::DrawSceneToShadowMap() {
	renderer.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(CommonVertex);
	UINT offset = 0;

	shadowInpLayout->Bind(renderer.GetDeviceContext());
	// shadowVBuffer->Bind(renderer.GetDeviceContext());
	shadowIBuffer->Bind(renderer.GetDeviceContext());
	shadowVShader->Bind(renderer.GetDeviceContext());
	shadowRast->Bind(renderer.GetDeviceContext());
	shadowTransforms->Bind(renderer.GetDeviceContext());
	for (SceneNode* node : scene.nodes) {
		for (size_t i = 0; i < node->bindables.size(); i++)
		{
			node->bindables[i]->Bind(renderer.GetDeviceContext());
		}
		
		renderer.GetDeviceContext()->PSSetShader(nullptr, nullptr, 0u);

		renderer.GetDeviceContext()->DrawIndexed(node->indicesNum, 0, 0);

	}
}
