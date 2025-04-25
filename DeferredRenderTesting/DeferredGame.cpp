#include "DeferredGame.h"

DeferredGame::DeferredGame()
{
	applicationName = L"DeferredGame";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();

	scene = Scene();

	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);

	renderer = new DeferredRenderer(&displayWindow);

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &DeferredGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &DeferredGame::HandleMouseMove);

	// GBufferPass
	GBufferPass* gBufferPass;
	{
		gBufferPass = new GBufferPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		renderer->SetMainCamera(gBufferPass->GetCamera());
		renderer->mainCamera->SetPosition({ 0, 0, -10 });

		renderer->AddPass(gBufferPass);
	}
	/*
	LightPass* gLightPass;
	{
		gLightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, gBufferPass->pGBuffer, gBufferPass->GetCamera());

		gLightPass->SetCamera(gBufferPass->GetCamera());

		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pNormalSRV.Get(), 0u));
		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pAlbedoSRV.Get(), 1u));
		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pWorldPosSRV.Get(), 2u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		gLightPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		//renderer->AddPass(gLightPass);
	}
	*/
	// MainColorPass
	{
		MainColorPass* colorPass = new MainColorPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		colorPass->SetCamera(renderer->GetMainCamera());

		//colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pDepthSRV.Get(), 0u));
		//colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pNormalSRV.Get(), 0u));
		colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pAlbedoSRV.Get(), 0u));
		//colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pLightSRV.Get(), 2u));

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		colorPass->AddPerFrameBind(new Bind::BlendState(renderer->GetDevice()));

		renderer->AddPass(colorPass);
	}

	TestCube* _tc = new TestCube(renderer->GetDevice(), 0.2, 0.2, 0.2, { 0,0,0 }, { 1,0,0,1 });
	_tc->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV4, XM_PIDIV4, 0));
	scene.AddNode(_tc);

	TestCube* _tc_2 = new TestCube(renderer->GetDevice(), 0.2f, 0.2f, 0.2f, { 0.5f, 0.5f, -0.3f }, { 0.9f, 0.7f, 0.5f, 1.0f });
	_tc_2->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.4, XM_PIDIV4 * 0.12, 0));
	_tc_2->speed = 3.0f;
	scene.AddNode(_tc_2);

	TestCube* _tc_3 = new TestCube(renderer->GetDevice(), 10.0f, 10.0f, 1.0f, { 0.0f, 0.0f, 3.0f }, { 0.3f, 0.7f, 0.5f, 1.0f });
	scene.AddNode(_tc_3);

	TestCube* _tc_4 = new TestCube(renderer->GetDevice(), 0.5f, 0.5f, 0.5f, { -1.0f, 1.0f, -0.6f }, { 0.1f, 0.7f, 0.9f, 1.0f });
	_tc_4->SetInitTransform(Matrix::CreateFromYawPitchRoll(XM_PIDIV2 * 0.3, XM_PIDIV4 * 0.52, 0));
	_tc_4->speed = 5.0f;
	scene.AddNode(_tc_4);

	PointLight* _pl_1 = new PointLight(renderer->GetDevice(), { -0.5f, 0.5f, -0.2f }, 3.0f, { 0.01f, 2.0f, 0.0f },
		{ 0.1, 0, 0, 1 }, { 1, 0, 0, 1 }, { 1, 0, 0, 16 });
	scene.AddNode(_pl_1);

	PointLight* _pl_2 = new PointLight(renderer->GetDevice(), { 1.0f, 0.5f, -0.4f }, 5.4f, { 0.01f, 1.0f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 0, 16 });
	scene.AddNode(_pl_2);

	DirectionalLight* _dl_1 = new DirectionalLight(renderer->GetDevice(), { 5.0f, 5.5f, -5.0f }, { -1.0f, -1.0f, 1.0f },
		{ 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 }, { 0.2f, 0.2f, 0.7f, 1 });
	scene.AddNode(_dl_1);
	
	FullScreenQuad* fsq = new FullScreenQuad(renderer->GetDevice());
	scene.AddNode(fsq);
	
	for (SceneNode* node : scene.nodes) {
		node->camera = renderer->GetMainCamera();
	}
}

DeferredGame::~DeferredGame()
{
}

void DeferredGame::Update(float deltaTime)
{
	physEngine->Update(deltaTime);
}

void DeferredGame::Render()
{
	renderer->RenderScene(scene);
}

void DeferredGame::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		renderer->mainCamera->MoveForward(deltaTime * 10.0f);
	}
	if (key == Keys::S)
	{
		renderer->mainCamera->MoveBackward(deltaTime * 10.0f);
	}
	if (key == Keys::A)
	{
		renderer->mainCamera->MoveLeft(deltaTime * 10.0f);
	}
	if (key == Keys::D)
	{
		renderer->mainCamera->MoveRight(deltaTime * 10.0f);
	}
	if (key == Keys::Space)
	{
		renderer->mainCamera->MoveUp(deltaTime * 10.0f);
	}

	if (key == Keys::LeftShift)
	{
		renderer->mainCamera->MoveDown(deltaTime * 10.0f);
	}
	
	if (key == Keys::Space)
	{
		//renderer->mainCamera->MoveDown(deltaTime * 10.0f);
		std::cout << "hello\n";
	}
}


void DeferredGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderer->mainCamera->RotateYaw(deltaTime * args.Offset.x * 0.1);
	renderer->mainCamera->RotatePitch(-deltaTime * args.Offset.y * 0.1);
}