#pragma once
#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>

#include "RenderingSystem.h"
#include "DisplayWindow.h"
#include "GBuffer.h"

class DeferredRenderer :
    public RenderingSystem
{
public:
    DeferredRenderer();
    DeferredRenderer(DisplayWindow* displayWin);

    // Inherited via RenderingSystem
    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;

    GBuffer gBuffer;

    std::vector<RenderPass*> passes;
};

