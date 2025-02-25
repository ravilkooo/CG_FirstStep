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
};

#endif // PONGGAME_H