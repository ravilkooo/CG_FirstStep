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
    CosmicBody* sun = new CosmicBody(0.3f, 0.5f, DirectX::XMFLOAT3(0.0f, 0.3f, 0.5f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), CosmicBody::PLANET_TYPE::SPHERE);
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

    focusedBody = moon;
    renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f));

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
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_1))
    {
        focusedBody = cosmicBodies[0];
        renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f));
        //renderer.camera.SwitchProjection();
    }
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_2))
    {
        focusedBody = cosmicBodies[1];
        renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f));
        //renderer.camera.SwitchProjection();
    }
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_3))
    {
        focusedBody = cosmicBodies[2];
        renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f));
        //renderer.camera.SwitchProjection();
    }
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D_4))
    {
        focusedBody = cosmicBodies[3];
        renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f));
        //renderer.camera.SwitchProjection();
    }
    else
    {

    }
    if (focusedBody) {
        /*std::cout << "(" << focusedBody->GetCenterLocation().x << ", "
            << focusedBody->GetCenterLocation().y << ", "
            << focusedBody->GetCenterLocation().z << ")\n";*/

        renderer.camera.Update(deltaTime, focusedBody->cb.worldMat);

        //renderer.camera.SwitchToOrbitalMode(scene.nodes[scene.nodes.size() - 3]->GetCenterLocation());
    }
    //renderer.camera.Update(deltaTime);
    // Обновление состояния игры
    physEngine->Update(deltaTime);
}

void SolarSystemGame::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}
