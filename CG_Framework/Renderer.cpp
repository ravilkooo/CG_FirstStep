#include "Renderer.h"

Renderer::Renderer() : device(nullptr), context(nullptr)
{
}

Renderer::Renderer(DisplayWindow* displayWin)
{
	Initialize(displayWin);
}

Renderer::~Renderer()
{
    //if (context) context->Release();
    //if (device) device->Release();
}

bool Renderer::Initialize(DisplayWindow* displayWin)
{
	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;

	LPCWSTR applicationName = L"SunshineEngine";
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	this->displayWindow = displayWindow;

	/*HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&device,
		nullptr,
		&context
	);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create D3D11 device");*/

	// swapChain

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = displayWin->hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		nullptr,
		&context);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Device with Swap Chain");

	// backBuffer

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer");

	// rtv

	hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// resourceManagers for buffers

	resourceManager = ResourceManager(GetDevice());

	// Load and compile shaders (move to other section?)
	shaderManager = ShaderManager(GetDevice());

	vertexShader = nullptr;
	pixelShader = nullptr;
	vsBlob = nullptr;
	if (!shaderManager.LoadVertexShader(L"./Shaders/MyVeryFirstShader.hlsl", &vertexShader, &vsBlob))
		std::cout << "Ujas!\n";
	if (!shaderManager.LoadPixelShader(L"./Shaders/MyVeryFirstShader.hlsl", &pixelShader))
		std::cout << "Ujas!\n";

	/*
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;
	hr = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr, // macros,
		nullptr  // include,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode);

	if (FAILED(hr)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			std::cout << L"MyVeryFirstShader.hlsl" << L"Missing Shader File\n";
		}

		return 0;
	}

	// Compile pixel shader 
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	ID3DBlob* pixelBC;
	ID3DBlob* errorPixelCode;
	hr = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros, // macros,
		nullptr, //include,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelBC,
		&errorPixelCode);

	// A vertex-shader/pixel-shader interface manages an executable program (a vertex/pixel shader) that controls the vertex-shader/pixel-shader stage
	// ? Интерфейс для управления шеёдерами?
	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);
	*/

	// Input layout handler

	inputAssembler = InputAssembler(GetDevice(), GetDeviceContext());
	
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

	inputAssembler.CreateInputLayout(inputElements, vsBlob);

	pipelineState = PipelineState(GetDevice(), GetDeviceContext());

	// 6. Create Set of Points

	// 7. Create Vertex and Index Buffers

	// Create vb
	vb = resourceManager.CreateVertexBuffer(points, sizeof(DirectX::XMFLOAT4) * std::size(points));

	// Create Set of indices

	// Create ib
	ib = resourceManager.CreateIndexBuffer(indices, sizeof(int) * std::size(indices));

	//pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	

    return true;
}

void Renderer::RenderScene(const Scene& scene)
{
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;
	totalTime += deltaTime;
	totalTime = totalTime > 1.0f ? totalTime - 1.0f : totalTime;

	/*for (size_t i = 0; i < 4; i++)
	{
		auto _x = points[2 * i].x;
		auto _y = points[2 * i].y;
		points[2 * i].x = _x*cos(0.1 * deltaTime) - _y * sin(0.1 * deltaTime);
		points[2 * i].y = _x*sin(0.1 * deltaTime) + _y * cos(0.1 * deltaTime);
	}*/

	D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, points, sizeof(DirectX::XMFLOAT4) * std::size(points));
	context->Unmap(vb, 0);
	

	//context->ClearState(); // Restore all default settings
	//context->RSSetState(rastState);
	
	// Create vb
	//vb = resourceManager.CreateVertexBuffer(points, sizeof(DirectX::XMFLOAT4) * std::size(points));

	// Create Set of indices

	// Create ib
	//ib = resourceManager.CreateIndexBuffer(indices, sizeof(int) * std::size(indices));

	//pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	// 8. Setup the IA stage

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	// 10. Setup Rasterizer Stage and Viewport

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	// 8. Setup the IA stage

	inputAssembler.SetInputLayout();
	inputAssembler.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	// 9. Set Vertex and Pixel Shaders

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	// 11. Set BackBuffer for Output
	context->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// 13. At the End of While(!isExitRequested) : Clear BackBuffer
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, color);

	// 14. At the End of While (!isExitRequested): Draw the Triangle
	context->DrawIndexed(6, 0, 0);
	//context->Draw(6, 0);

	// зачем эта строчка? 
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// 15. At the End of While (!isExitRequested): Present the Result
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);

	//vertexShader->Release();
	//pixelShader->Release();

	
}

ID3D11Device* Renderer::GetDevice()
{
	return device.Get();
}

ID3D11DeviceContext* Renderer::GetDeviceContext()
{
	return context.Get();
}