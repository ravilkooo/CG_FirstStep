#include "Game.h"

Game::Game()
{
    Initialize();
}

Game::~Game()
{
    // Освобождение ресурсов
}

void Game::Run()
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

void Game::Initialize()
{

    applicationName = L"SunShine_0";
    hInstance = GetModuleHandle(nullptr);

    timer = GameTimer();

    physEngine = PhysicsEngine();

    displayWindow = DisplayWindow(applicationName, hInstance, 800, 800);

    renderer = Renderer(&displayWindow);
    
}

void Game::Update(float deltaTime)
{
    // Обновление состояния игры
}

void Game::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}