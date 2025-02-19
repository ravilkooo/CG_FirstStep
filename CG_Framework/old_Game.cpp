#include "old_Game.h"

Game::Game(LPCWSTR name)
{
	display = DisplayWin32(name, 800, 800, nullptr);
	

}

void Game::Initialize()
{

}

void Game::MessageHandler()
{
}

void Game::PrepareFrame()
{
}

void Game::PrepareResources()
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = display.clientWidth;
	swapDesc.BufferDesc.Height = display.clientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = display.hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	CreateBackBuffer();

	DirectX::XMFLOAT4 points_0[6] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	};

	DirectX::XMFLOAT4 points_1[6] = {
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	components.push_back(new TriangleComponent(this, points_0));
	components.push_back(new TriangleComponent(this, points_1));

	for (auto game_comp : components)
	{
		game_comp->Initialize();
	}

	//// 6. Create Set of Points
	//DirectX::XMFLOAT4 points[8] = {
	//	DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
	//	DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
	//	DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	//	DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	//};

	// Timer for changing color and measure FPS
	prevTime = std::chrono::steady_clock::now();
	totalTime = 0;
	frameCount = 0;

	// For handling messages (including exit)
	msg = {};
	isExitRequested = false;
}

void Game::RestoreTarget()
{
}

void Game::Update()
{
	// Handle the windows messages.
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// If windows signals to end the application then exit out.
	if (msg.message == WM_QUIT) {
		isExitRequested = true;
	}

	for (auto game_comp : components)
	{
		game_comp->Update_0();
	}

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(display.clientWidth);
	viewport.Height = static_cast<float>(display.clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	for (auto game_comp : components)
	{
		game_comp->Update();
	}

	// For changing color and measure FPS
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;

	totalTime += deltaTime;
	frameCount++;

	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;

		totalTime -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(display.hWnd, text);

		frameCount = 0;
	}

	// 11. Set BackBuffer for Output
	context->OMSetRenderTargets(1, &(renderView), nullptr);

	// 13. At the End of While(!isExitRequested) : Clear BackBuffer
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderView, color);

	for (auto game_comp : components)
	{
		game_comp->Draw();
	}

	// зачем эта строчка? 
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// 15. At the End of While (!isExitRequested): Present the Result
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::UpdateInternal()
{
}

void Game::CreateBackBuffer()
{
	auto res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backBuffer, nullptr, &renderView);


}

void Game::DestroyResources()
{
}

void Game::Draw()
{
}

void Game::EndFrame()
{
}


void Game::Run()
{
	PrepareResources(); // ?
	while (!isExitRequested)
	{
		RestoreTarget();
		MessageHandler();
		UpdateInternal();
		Update();
		PrepareFrame();
		Draw();
		EndFrame();

		UpdateInternal();
	}
}

#pragma region TriangleComponent

Game::TriangleComponent::TriangleComponent(Game* game)
{
	this->game = game;
}

Game::TriangleComponent::TriangleComponent(Game* game, DirectX::XMFLOAT4* vertices)
{
	this->game = game;
	for (size_t i = 0; i < 6; i++)
	{
		this->vertices[i] = vertices[i];
	}
}

void Game::TriangleComponent::Initialize()
{
	vertexShaderByteCode = nullptr;
	errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderByteCode,
		&errorVertexCode);

	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(game->display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return;
	}


	// Compile pixel shader 
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode,
		&errorPixelCode);

	game->device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr, &vertexShader);

	game->device->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr, &pixelShader);

	// Описываем какие элементы будут на входе
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};
	
	game->device->CreateInputLayout(
		inputElements,
		2,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&layout);

	// Describe vertex buffer
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(vertices);

	// Specify data for initializing a subresource
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create vb
	game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);


	// Вынести в update? Потому что у каждого тр-ка посвоему?
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = game->device->CreateRasterizerState(&rastDesc, &rastState);
	
	game->context->RSSetState(rastState);
}

void Game::TriangleComponent::Draw()
{

	// 14. At the End of While (!isExitRequested): Draw the Triangle
	game->context->Draw(6, 0);

}

void Game::TriangleComponent::Update_0()
{
	/*std::cout << "zdraste\n";
	for (size_t i = 0; i < 6; i++)
	{
		std::cout << vertices[i].x << "; ";
	}
	std::cout << "\n";*/


	game->context->ClearState(); // Restore all default settings
	game->context->RSSetState(rastState);

}

void Game::TriangleComponent::Update()
{
	game->context->IASetInputLayout(layout);
	game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	game->context->VSSetShader(vertexShader, nullptr, 0);
	game->context->PSSetShader(pixelShader, nullptr, 0);

}

void Game::TriangleComponent::DestroyResources()
{
	vertexShader->Release();
	pixelShader->Release();
}

#pragma endregion TriangleComponent

#pragma region GameComponent

Game::GameComponent::GameComponent()
{
}

Game::GameComponent::GameComponent(Game* game)
{
}

void Game::GameComponent::Initialize()
{
}

void Game::GameComponent::Draw()
{
}

void Game::GameComponent::Reload()
{
}

void Game::GameComponent::DestroyResources()
{
}

#pragma endregion GameComponent
