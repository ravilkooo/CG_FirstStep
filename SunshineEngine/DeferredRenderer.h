#pragma once
#include "RenderingSystem.h"
class DeferredRenderer :
    public RenderingSystem
{
    // Inherited via RenderingSystem
    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;
};

