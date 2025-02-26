#ifndef PONGGAME_H
#define PONGGAME_H

#include "Border.h"
#include "Racket.h"
#include "Ball.h"
#include "Gates.h"

#include "Game.h";

class PongGame : public Game
{
public:
    PongGame();
    ~PongGame();

    void Run();

    void Initialize();
    void Update(float deltaTime);
    void Render();

    bool CheckBorderCollision();
private:
    Border* border;
    Ball* ball;
};

#endif // PONGGAME_H