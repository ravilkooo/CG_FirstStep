#include "SolarSystemGame.h"



SolarSystemGame::SolarSystemGame()
{
	Initialize();
}

SolarSystemGame::~SolarSystemGame()
{
}


void SolarSystemGame::Initialize()
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
	CosmicBody* sun = new CosmicBody(sun_rad, 0.2f, DirectX::XMFLOAT3(0.0f, 0.3f, 0.5f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE);
	cosmicBodies.push_back(sun);

	accum_dist += planet_gap + merc_rad;
	CosmicBody* mercury = new CosmicBody(merc_rad, 0.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::GEOSPHERE,
		sun, accum_dist, 0.5f);
	cosmicBodies.push_back(mercury);

	accum_dist += merc_rad + planet_gap + venus_rad;
	CosmicBody* venus = new CosmicBody(venus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		sun, accum_dist, 1.0f);
	cosmicBodies.push_back(venus);

	accum_dist += venus_rad + planet_gap + (2 * moon_rad + moon_gap + earth_rad);
	CosmicBody* earth = new CosmicBody(earth_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 0.7f);
	CosmicBody* moon = new CosmicBody(moon_rad, 0.6f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		earth, earth_rad + moon_gap + moon_rad, 1.8f);
	cosmicBodies.push_back(earth);
	cosmicBodies.push_back(moon);

	accum_dist += (2 * moon_rad + moon_gap + earth_rad) + planet_gap + (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad);

	//std::cout << mars_rad + moon_gap + fobos_rad << "\n";

	CosmicBody* mars = new CosmicBody(mars_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 0.8);
	CosmicBody* fobos = new CosmicBody(fobos_rad, 0.6f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.4f, 0.1f, 0.1f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		mars, mars_rad + moon_gap + fobos_rad, 2.1f);
	CosmicBody* deimos = new CosmicBody(deimos_rad, 0.4f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		mars, 2 * fobos_rad + deimos_rad + 2 * moon_gap + mars_rad, 2.9f);
	cosmicBodies.push_back(mars);
	cosmicBodies.push_back(fobos);
	cosmicBodies.push_back(deimos);

	accum_dist += (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad) + moon_gap + merc_rad;

	UINT asteroid_cnt = 20;
	for (size_t i = 0; i < asteroid_cnt; i++)
	{
		cosmicBodies.push_back(
			new CosmicBody(merc_rad, 0.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.3f, 0.2f, 0.2f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
				sun, accum_dist, 0.5f)
		);
		cosmicBodies[cosmicBodies.size() - 1]->rotationAngle = ((7 * i) % asteroid_cnt) * XM_2PI / asteroid_cnt;
		cosmicBodies[cosmicBodies.size() - 1]->orbitAngle = i * XM_2PI / asteroid_cnt;
	}

	accum_dist += (merc_rad)+planet_gap + (jupiter_rad);

	CosmicBody* jupiter = new CosmicBody(jupiter_rad, 0.3f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE,
		sun, accum_dist, 1.4f);
	cosmicBodies.push_back(jupiter);

	accum_dist += (jupiter_rad)+planet_gap + (saturn_ring_rad + planet_gap);

	CosmicBody* saturn = new CosmicBody(saturn_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.2f, 1.0f), CosmicBody::PLANET_TYPE::CUBE,
		sun, accum_dist, 1.73f);
	CosmicBody* saturn_ring = new CosmicBody(saturn_ring_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::RING,
		saturn, 0.0f, 2.4f);
	cosmicBodies.push_back(saturn);
	cosmicBodies.push_back(saturn_ring);


	accum_dist += (saturn_ring_rad + planet_gap) + planet_gap + (saturn_ring_rad + planet_gap);


	CosmicBody* uranus = new CosmicBody(uranus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::GEOSPHERE,
		sun, accum_dist, 1.4f);
	CosmicBody* uranus_ring = new CosmicBody(uranus_ring_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::RING,
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

	displayWindow = DisplayWindow(applicationName, hInstance, winWidth, winHeight);
	inputHandler = displayWindow.GetInputHandler();

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	focusedBody = moon;
	//renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);

	for (auto node : scene.nodes)
	{
		node->LoadAndCompileShader(renderer.shaderManager);
		node->InitBuffers(renderer.resourceManager);
		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}
	// Инициализация камеры
	//camera.SetPosition(DirectX::XMFLOAT3(0.0f, 10.0f, -20.0f));
	//camera.SetTarget(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	//camera.SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
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
		float deltaTime = timer.GetDeltaTime();
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
	// Получаем InputHandler из DisplayWindow
	inputHandler = displayWindow.GetInputHandler();

	// Управление ракеткой игрока
	if (inputHandler->IsKeyDown(InputHandler::KeyCode::UP))
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveUp(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::DOWN))
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveDown(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::RIGHT))
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveRight(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::LEFT))
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveLeft(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::Q))
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveForward(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::E))
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveBackward(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::W))
	{
		renderer.camera.RotatePitch(-deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::S))
	{
		renderer.camera.RotatePitch(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::A))
	{
		renderer.camera.RotateYaw(-deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D))
	{
		renderer.camera.RotateYaw(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_1))
	{
		focusedBody = cosmicBodies[0];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_2))
	{
		focusedBody = cosmicBodies[1];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_3))
	{
		focusedBody = cosmicBodies[2];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_4))
	{
		focusedBody = cosmicBodies[3];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	else if (inputHandler->IsKeyClicked(InputHandler::KeyCode::COMMA))
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBodyIdx = (focusedBodyIdx - 1 + cosmicBodies.size()) % cosmicBodies.size();
			focusedBody = cosmicBodies[focusedBodyIdx];
			renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
		}
	}
	else if (inputHandler->IsKeyClicked(InputHandler::KeyCode::PERIOD))
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBodyIdx = (focusedBodyIdx + 1) % cosmicBodies.size();
			focusedBody = cosmicBodies[focusedBodyIdx];
			renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
		}
	}
	else if (inputHandler->IsKeyClicked(InputHandler::KeyCode::B))
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			renderer.camera.SwitchProjection();
		}
	}
	else if (inputHandler->IsKeyClicked(InputHandler::KeyCode::SPACE))
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBody = nullptr;
			renderer.camera.SwitchToFPSMode();
			//renderer.camera.SwitchProjection();
		}
	}
	else
	{

	}
	buttonTimer += deltaTime;
	//renderer.camera.Update(deltaTime);
	// Обновление состояния игры
	physEngine->Update(deltaTime);

	if (focusedBody) {
		/*std::cout << "(" << focusedBody->GetCenterLocation().x << ", "
			<< focusedBody->GetCenterLocation().y << ", "
			<< focusedBody->GetCenterLocation().z << ")\n";*/

		renderer.camera.Update(deltaTime, focusedBody->worldMat);

		//renderer.camera.SwitchToOrbitalMode(scene.nodes[scene.nodes.size() - 3]->GetCenterLocation());
	}
}

void SolarSystemGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}
