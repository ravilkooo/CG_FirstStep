#ifndef SOLARSYS_H
#define SOLARSYS_H

#include <Game.h>
#include "SolarSystemPhysics.h"
#include "CosmicBody.h"
#include <Camera.h>

class SolarSystemGame :
    public Game
{
public:
    SolarSystemGame();
    ~SolarSystemGame();

    void Run() override;

    void Update(float deltaTime) override;
    void Render() override;

    UINT winWidth = 800;
    UINT winHeight = 800;

private:
    //Camera camera;
    std::vector<CosmicBody*> cosmicBodies;
    CosmicBody* focusedBody = nullptr;
    UINT focusedBodyIdx = 0;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};


#endif
