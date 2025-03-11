#ifndef KATAMARIGAME_H
#define KATAMARIGAME_H

#include <Game.h>
#include "StickyBall.h"

class KatamariGame :
    public Game
{
public:
    KatamariGame();
    ~KatamariGame();

    void Initialize() override;

    void Run() override;
    void Update(float deltaTime) override;
    void Render() override;

    InputHandler* inputHandler;

    StickyBall ball;

};

#endif
