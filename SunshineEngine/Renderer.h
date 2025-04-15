#ifndef RENDERER_H
#define RENDERER_H


#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>
#include "DisplayWindow.h"

#include "Scene.h"
#include "RenderPass.h"

#include <chrono>

/*
RenderPass (Color, Shadow,..)



1. PerFrameBindable
2. PerObjectBindable
2.1. Base (VertexBuufer, IndexBuffer, ...)
2.2. Specific for current render pass

RenderPass -?- Drawable

Drawable has Bindables for each RenderPass (Technique)

Drawable {}
map<string, Technique*> techniques;

RenderPass {}
string tag;

void Pass() {}
BindPerFrame();
for every Drawable obj:
    obj.BindBase();
    obj.BindTecnique(RenderPass.GetTag());
    obj.Draw();

*/

class Renderer
{
    friend class Bindable;
public:
    Renderer();
    Renderer(DisplayWindow* displayWin);
    ~Renderer();

    void RenderScene(const Scene& scene);

    
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();
    ID3D11Texture2D* GetBackBuffer();
    void AddPass(RenderPass* pass);

    void SetMainCamera(Camera* camera);
    Camera* GetMainCamera();

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };
    
    DisplayWindow* displayWindow;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    float totalTime;

    std::vector<RenderPass*> passes;

    Camera* mainCamera;

};

#endif // RENDERER_H