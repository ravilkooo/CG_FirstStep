#ifndef KATAMARIGAME_H
#define KATAMARIGAME_H

#include <Game.h>
#include "StickyBall.h"
#include "Floor.h"
#include "CollectibleObject.h"

class KatamariGame :
    public Game
{
public:
    KatamariGame();
    ~KatamariGame();

    void Run() override;
    void Update(float deltaTime) override;
    void Render() override;

    std::vector<CollectibleObject> collectibles;
    void SpawnCollectibles();

    InputHandler* inputHandler;

    StickyBall ball;
    Floor floor;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};

#endif
