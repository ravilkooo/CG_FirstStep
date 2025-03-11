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

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;

    InputHandler* inputHandler;

    float pressTime = 0.1f;
    float buttonTimer = 10.0f;

private:
    //Camera camera;
    std::vector<CosmicBody*> cosmicBodies;
    CosmicBody* focusedBody = nullptr;
    UINT focusedBodyIdx = 0;
};


#endif
