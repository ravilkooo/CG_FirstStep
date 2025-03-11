#include "KatamariGame.h"

KatamariGame::KatamariGame()
{
	Initialize();
}

KatamariGame::~KatamariGame()
{
}

void KatamariGame::Initialize()
{
	applicationName = L"SolarSystem";
	hInstance = GetModuleHandle(nullptr);

	timer = GameTimer();

	scene = Scene();

	// Добавление объектов на сцену
	/*for (auto body : cosmicBodies)
	{
		scene.AddNode(body);
	}*/


	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(applicationName, hInstance, 800, 800);
	inputHandler = displayWindow.GetInputHandler();

	renderer = Renderer(&displayWindow);

	for (auto node : scene.nodes)
	{
		node->LoadAndCompileShader(renderer.shaderManager);
		node->InitBuffers(renderer.resourceManager);
		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}

	//renderer.camera.SwitchToOrbitalMode(ball.GetCenterLocation(), Vector3(0.0f ), focusedBody->radius);
}

void KatamariGame::Run()
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

void KatamariGame::Update(float deltaTime)
{
	physEngine->Update(deltaTime);
}

void KatamariGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}