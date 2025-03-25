#include "ShaderManager.h"

ShaderManager::ShaderManager() : device(nullptr)
{
}

ShaderManager::ShaderManager(ID3D11Device* device) : device(device)
{
}

bool ShaderManager::LoadVertexShader(LPCWSTR filePath, ID3D11VertexShader** vertexShader, ID3DBlob** shaderBlob)
{

	ID3DBlob* errorVertexCode = nullptr;
    HRESULT hr = D3DCompileFromFile(
		filePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		shaderBlob,
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
			std::cout << filePath << L" - Missing Shader File\n";
		}

		return 0;
	}
    
    if (FAILED(hr)) return false;

    hr = device->CreateVertexShader(
		(*shaderBlob)->GetBufferPointer(),
		(*shaderBlob)->GetBufferSize(),
		nullptr,
		vertexShader);

    return SUCCEEDED(hr);
}

bool ShaderManager::LoadPixelShader(LPCWSTR filePath, ID3D11PixelShader** pixelShader)
{
    ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorPixelCode;
    HRESULT hr = D3DCompileFromFile(
		filePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBlob,
		&errorPixelCode);
	
	if (FAILED(hr)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());

			std::cout << compileErrors << " - // -- " << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			std::cout << filePath << L" - Missing Shader File\n";
		}

		return 0;
	}
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		pixelShader);
    shaderBlob->Release();
    return SUCCEEDED(hr);
}

ShaderManager::~ShaderManager()
{
}

//ShaderManager::~ShaderManager()
//{
//    for (auto& shader : vertexShaders)
//    {
//        shader.second->Release();
//    }
//    for (auto& shader : pixelShaders)
//    {
//        shader.second->Release();
//    }
//}
//
//bool ShaderManager::LoadShader(const std::string& name, const std::string& filePath)
//{
//    // Загрузка и компиляция шейдеров
//    return true;
//}
//
//ID3D11VertexShader* ShaderManager::GetVertexShader(const std::string& name)
//{
//    return vertexShaders[name];
//}
//
//ID3D11PixelShader* ShaderManager::GetPixelShader(const std::string& name)
//{
//    return pixelShaders[name];
//}