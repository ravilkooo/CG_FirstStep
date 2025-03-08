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

    // Создание планет и лун
    CosmicBody* sun = new CosmicBody(0.3f, 1.0f, DirectX::XMFLOAT3(0.0f, 0.3f, 0.5f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE);
    CosmicBody* earth = new CosmicBody(0.1f, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE, sun, 0.5f, 1.0f);
    CosmicBody* moon = new CosmicBody(0.05f, 0.6f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), CosmicBody::PLANET_TYPE::CUBE, earth, 0.1f, 2.0f);

    CosmicBody* mars = new CosmicBody(0.05f, 0.7f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::GEOSPHERE, sun, 0.3f, 0.5f);

    cosmicBodies.push_back(sun);
    cosmicBodies.push_back(earth);
    cosmicBodies.push_back(moon);
    cosmicBodies.push_back(mars);

    // Добавление объектов на сцену
    for (auto body : cosmicBodies)
    {
        scene.AddNode(body);
    }

    physEngine = new SolarSystemPhysics(&scene);

    displayWindow = DisplayWindow(applicationName, hInstance, 800, 800);
    inputHandler = displayWindow.GetInputHandler();

    renderer = Renderer(&displayWindow);

    for (auto node : scene.nodes)
    {
        node->LoadAndCompileShader(renderer.shaderManager);
        node->InitBuffers(renderer.resourceManager);
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
    }
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::DOWN))
    {
        //std::cout << "MoveDown\n";
    }
    else
    {

    }

    // Обновление состояния игры
    physEngine->Update(deltaTime);
}

void SolarSystemGame::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}
