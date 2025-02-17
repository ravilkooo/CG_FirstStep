#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>

#include <vector>
#include "DisplayWin32.h"
#include "InputDevice.h"
#include "GameComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game {

	ID3D11Texture2D* backBuffer; // backTex ?
	ID3D11DeviceContext* context;
	//DebugAnnotation;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	//Instance;
	//Name;
	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	//RenderSRV; // ? render service?
	//RenderView; // ID3D11RenderTargetView* ?
	//ScreenResized; // bool ?
	//StartTime;
	IDXGISwapChain* swapChain;
	float totalTime;

	DisplayWin32 Display;

	InputDevice InputDevice;

	std::vector<GameComponent> Components;

	Game();

	void Initialize();

	void MessageHandler();

	void PrepareFrame();

	void PrepareResources();

	void RestoreTarget();

	void Run();

	void Update();

	void UpdateInternal();

	void CreateBackBuffer();

	void DestroyResources();

	void Draw();

	void EndFrame();


};

#endif