#include "PongGame.h"


PongGame::PongGame()
{
    Initialize();
}

PongGame::~PongGame()
{
}

void PongGame::Run()
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
            float fps = frameCount * 1. / totalTime;

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

void PongGame::Initialize()
{

    applicationName = L"SunShine_0";
    hInstance = GetModuleHandle(nullptr);

    timer = GameTimer();

    border = new Border();
    ball = new Ball(DirectX::XMFLOAT4(0., 0., 0.25, 1), 0.1);
    
    racket_player = new Racket(DirectX::XMFLOAT4(-0.8, 0., 0.25, 1), 0.1, 0.4, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
    racket_player->angle_velocity = 0.1f;

    racket_AI = new Racket(DirectX::XMFLOAT4(0.8, 0., 0.25, 1), 0.1, 0.4, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
    racket_AI->angle_velocity = 0.9f;
    //racket_AI->normal = { -1.f, 0.f, 0.f, 0.f };

    gates_player = new Gates(DirectX::XMFLOAT4(-0.8f, 0.f, 0.25f, 1.f), 0.1f);
    gates_player->normal = { 1.f, 0.f, 0.f, 0.f };

    gates_AI = new Gates(DirectX::XMFLOAT4(0.8, 0.f, 0.25f, 1.f), -0.1f);
    gates_AI->normal = { -1.f, 0.f, 0.f, 0.f };

    scene = Scene();
    scene.AddNode(border);
    scene.AddNode(gates_player);
    scene.AddNode(gates_AI);

    scene.AddNode(ball);
    scene.AddNode(racket_player);
    scene.AddNode(racket_AI);

    physEngine = new PongPhysics(&scene, ball, racket_player,
        racket_AI, gates_player, gates_AI, border);

    displayWindow = DisplayWindow(applicationName, hInstance, 800, 800);
    inputHandler = displayWindow.GetInputHandler();

    renderer = Renderer(&displayWindow);

    for (auto node : scene.nodes)
    {
        node->LoadAndCompileShader(renderer.shaderManager);
        node->InitBuffers(renderer.resourceManager);
        //std::cout << "f1\n";
    }
}

void PongGame::Update(float deltaTime)
{
    // Получаем InputHandler из DisplayWindow
    inputHandler = displayWindow.GetInputHandler();

    // Управление ракеткой игрока
    if (inputHandler->IsKeyDown(InputHandler::KeyCode::UP))
    {
        //std::cout << "MoveUp\n";
        racket_player->MoveUp();
    }
    else if (inputHandler->IsKeyDown(InputHandler::KeyCode::DOWN))
    {
        //std::cout << "MoveDown\n";
        racket_player->MoveDown();
    }
    else
    {
        racket_player->Stop();
    }

    // Обновление состояния игры
    physEngine->Update(deltaTime);
}

void PongGame::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}
