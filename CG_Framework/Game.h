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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game {

public:
	class GameComponent {
	public:
		Game* game;

		GameComponent();

		GameComponent(Game* game);

		virtual void Initialize();

		virtual void Draw();

		void Reload();

		virtual void Update() = 0;
		virtual void Update_0() = 0;

		void DestroyResources();
	};

	class TriangleComponent : public GameComponent {
	private:
		ID3D11InputLayout* layout;

		ID3D11PixelShader* pixelShader;
		ID3DBlob* pixelShaderByteCode; // pixelBC ?
		ID3DBlob* errorPixelCode;


		//DirectX::XMFLOAT4* points; // отличие от vertices? Может это для multisampling?

		ID3D11RasterizerState* rastState;

		ID3D11VertexShader* vertexShader;
		ID3DBlob* vertexShaderByteCode; // vertexBC?
		ID3DBlob* errorVertexCode; // my
		DirectX::XMFLOAT4 vertices[6]; // отличие от points?

		// my

		ID3D11Buffer* vb;

	public:
		TriangleComponent(Game* game);
		TriangleComponent(Game* game, DirectX::XMFLOAT4* vertices);

		void Initialize();

		void Draw();

		void Update();
		void Update_0();

		void DestroyResources();
	};

	class InputDevice {
	private:
		Game* game;
		//keys;

		void OnKeyDown();
		void OnMouseRemoved();

	public:
		//MouseOffset;
		//MousePosition;
		//MouseWheelDown;
		;

		void AddPressedKey();
		void IsKeyDown();
		void RemovePressedKey();

	};

	ID3D11Texture2D* backBuffer; // backTex ?
	ID3D11DeviceContext* context;
	//DebugAnnotation;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	//Instance;
	LPCWSTR name;
	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	//RenderSRV; // ? render service?
	ID3D11RenderTargetView* renderView; // ID3D11RenderTargetView* rtv?
	//ScreenResized; // bool ?
	//StartTime;
	IDXGISwapChain* swapChain;
	float totalTime;

	DisplayWin32 display;

	InputDevice inputDevice;

	std::vector<GameComponent*> components;

	Game(LPCWSTR name);

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

	// my
private:

	MSG msg = {};
	bool isExitRequested = false;
	unsigned int frameCount = 0;

	ID3D11RasterizerState* rastState;
	ID3D11InputLayout* layout;
	ID3D11Buffer* ib;
	ID3D11Buffer* vb;


	UINT* strides;
	UINT* offsets;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

};

#endif