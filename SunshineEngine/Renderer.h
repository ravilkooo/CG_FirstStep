#ifndef RENDERER_H
#define RENDERER_H


#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>
#include "RenderingSystem.h"

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

class Renderer : public RenderingSystem
{
    friend class Bindable;
public:
    Renderer();
    Renderer(DisplayWindow* displayWin);
    ~Renderer();

    void RenderScene(const Scene& scene);

    void AddPass(RenderPass* pass) override;

    void SetMainCamera(Camera* camera);
    Camera* GetMainCamera();

protected:

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    float totalTime;

    std::vector<RenderPass*> passes;
};

#endif // RENDERER_H