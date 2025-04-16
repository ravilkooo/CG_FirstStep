#include "SolarSystemGame.h"

SolarSystemGame::SolarSystemGame()
{
	applicationName = L"SolarSystem";
	hInstance = GetModuleHandle(nullptr);

	timer = GameTimer();

	scene = Scene();

	float accum_dist = 0.0f;
	float planet_gap = 0.1f;
	float moon_gap = 0.01f;

	float sun_rad = 0.3f;
	float merc_rad = 0.01f;
	float venus_rad = 0.04f;

	float earth_rad = 0.07f;
	float moon_rad = 0.01f;

	float mars_rad = 0.05f;
	float fobos_rad = 0.007f;
	float deimos_rad = 0.005f;

	float jupiter_rad = 0.1f;

	float saturn_rad = 0.15f;
	float saturn_ring_rad = 0.22f;

	float uranus_rad = 0.09f;
	float uranus_ring_rad = 0.11f;


	// Создание планет и лун
	accum_dist = sun_rad;
	CosmicBody* sun = new CosmicBody(renderer.GetDevice(), sun_rad, 0.2f, DirectX::XMFLOAT3(0.0f, 0.3f, 0.5f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE);
	cosmicBodies.push_back(sun);

	accum_dist += planet_gap + merc_rad;
	CosmicBody* mercury = new CosmicBody(renderer.GetDevice(), merc_rad, 1.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::GEOSPHERE,
		sun, accum_dist, 10.5f);
	cosmicBodies.push_back(mercury);

	accum_dist += merc_rad + planet_gap + venus_rad;
	CosmicBody* venus = new CosmicBody(renderer.GetDevice(), venus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		sun, accum_dist, 1.0f);
	cosmicBodies.push_back(venus);

	accum_dist += venus_rad + planet_gap + (2 * moon_rad + moon_gap + earth_rad);
	CosmicBody* earth = new CosmicBody(renderer.GetDevice(), earth_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 0.7f);
	CosmicBody* moon = new CosmicBody(renderer.GetDevice(), moon_rad, 0.6f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		earth, earth_rad + moon_gap + moon_rad, 1.8f);
	cosmicBodies.push_back(earth);
	cosmicBodies.push_back(moon);

	accum_dist += (2 * moon_rad + moon_gap + earth_rad) + planet_gap + (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad);

	//std::cout << mars_rad + moon_gap + fobos_rad << "\n";

	CosmicBody* mars = new CosmicBody(renderer.GetDevice(), mars_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 0.8);
	CosmicBody* fobos = new CosmicBody(renderer.GetDevice(), fobos_rad, 0.6f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.4f, 0.1f, 0.1f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		mars, mars_rad + moon_gap + fobos_rad, 2.1f);
	CosmicBody* deimos = new CosmicBody(renderer.GetDevice(), deimos_rad, 0.4f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		mars, 2 * fobos_rad + deimos_rad + 2 * moon_gap + mars_rad, 2.9f);
	cosmicBodies.push_back(mars);
	cosmicBodies.push_back(fobos);
	cosmicBodies.push_back(deimos);

	accum_dist += (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad) + moon_gap + merc_rad;

	UINT asteroid_cnt = 20;
	for (size_t i = 0; i < asteroid_cnt; i++)
	{
		cosmicBodies.push_back(
			new CosmicBody(renderer.GetDevice(), merc_rad, 0.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.3f, 0.2f, 0.2f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
				sun, accum_dist, 0.5f)
		);
		cosmicBodies[cosmicBodies.size() - 1]->rotationAngle = ((7 * i) % asteroid_cnt) * XM_2PI / asteroid_cnt;
		cosmicBodies[cosmicBodies.size() - 1]->orbitAngle = i * XM_2PI / asteroid_cnt;
	}

	accum_dist += (merc_rad)+planet_gap + (jupiter_rad);

	CosmicBody* jupiter = new CosmicBody(renderer.GetDevice(), jupiter_rad, 0.3f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 1.4f);
	cosmicBodies.push_back(jupiter);

	accum_dist += (jupiter_rad)+planet_gap + (saturn_ring_rad + planet_gap);

	CosmicBody* saturn = new CosmicBody(renderer.GetDevice(), saturn_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.2f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		sun, accum_dist, 1.73f);
	CosmicBody* saturn_ring = new CosmicBody(renderer.GetDevice(), saturn_ring_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::RING,
		saturn, 0.0f, 2.4f);
	cosmicBodies.push_back(saturn);
	cosmicBodies.push_back(saturn_ring);


	accum_dist += (saturn_ring_rad + planet_gap) + planet_gap + (saturn_ring_rad + planet_gap);


	CosmicBody* uranus = new CosmicBody(renderer.GetDevice(), uranus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::GEOSPHERE,
		sun, accum_dist, 1.4f);
	CosmicBody* uranus_ring = new CosmicBody(renderer.GetDevice(), uranus_ring_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::RING,
		uranus, 0.0f, 5.1f);
	uranus_ring->orbitalAxis = Vector3(0.0f, cosf(XM_PIDIV2 * 0.9), sinf(XM_PIDIV2 * 0.9));
	cosmicBodies.push_back(uranus);
	cosmicBodies.push_back(uranus_ring);


	// Добавление объектов на сцену
	for (auto body : cosmicBodies)
	{
		scene.AddNode(body);
	}

	physEngine = new SolarSystemPhysics(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);
	// inputHandler = displayWindow.GetInputHandler();

	renderer = ForwardRenderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	focusedBody = moon;

	for (auto node : scene.nodes)
	{
		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	renderer.AddPerFrameBind(new Bind::DepthStencilState(renderer.GetDevice(), depthStencilDesc));

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &SolarSystemGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &SolarSystemGame::HandleMouseMove);
}


void SolarSystemGame::Run()
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

			//std::cout << frameCount << "\n";

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

void SolarSystemGame::Update(float deltaTime)
{
	// My custom
	// Получаем InputHandler из DisplayWindow
	// inputHandler = displayWindow.GetInputHandler();

	physEngine->Update(deltaTime);

	if (focusedBody) {

		renderer.camera.Update(deltaTime, focusedBody->worldMat);
	}
}

void SolarSystemGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}


SolarSystemGame::~SolarSystemGame()
{
}

void SolarSystemGame::HandleKeyDown(Keys key) {
	if (key == Keys::Up)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveUp(deltaTime);
	}
	if (key == Keys::Down)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveDown(deltaTime);
	}
	if (key == Keys::Right)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveRight(deltaTime);
	}
	if (key == Keys::Left)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveLeft(deltaTime);
	}
	if (key == Keys::E)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveForward(deltaTime);
	}
	if (key == Keys::Q)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveBackward(deltaTime);
	}
	if (key == Keys::W)
	{
		renderer.camera.RotatePitch(deltaTime);
	}
	if (key == Keys::S)
	{
		renderer.camera.RotatePitch(-deltaTime);
	}
	if (key == Keys::A)
	{
		renderer.camera.RotateYaw(-deltaTime);
	}
	if (key == Keys::D)
	{
		renderer.camera.RotateYaw(deltaTime);
	}
	if (key == Keys::D1)
	{
		focusedBody = cosmicBodies[0];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D2)
	{
		focusedBody = cosmicBodies[1];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D3)
	{
		focusedBody = cosmicBodies[2];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D4)
	{
		focusedBody = cosmicBodies[3];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::OemComma)
	{
		focusedBodyIdx = (focusedBodyIdx - 1 + cosmicBodies.size()) % cosmicBodies.size();
		focusedBody = cosmicBodies[focusedBodyIdx];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::OemPeriod)
	{
		focusedBodyIdx = (focusedBodyIdx + 1) % cosmicBodies.size();
		focusedBody = cosmicBodies[focusedBodyIdx];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::B)
	{
		renderer.camera.SwitchProjection();
	}
	if (key == Keys::Space)
	{
		focusedBody = nullptr;
		renderer.camera.SwitchToFPSMode();
	}

}

void SolarSystemGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderer.camera.RotateYaw(deltaTime * args.Offset.x * 0.1);
	renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.1);
}
