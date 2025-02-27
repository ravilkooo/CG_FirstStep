#ifndef PONGGAME_H
#define PONGGAME_H

#include <Game.h>;

#include "Border.h"
#include "Racket.h"
#include "Ball.h"
#include "Gates.h"
#include "PongPhysics.h"


class PongGame : public Game
{
public:
    PongGame();
    ~PongGame();

    void Run();

    void Initialize();
    void Update(float deltaTime);
    void Render();

    Ball* ball;
    Racket* racket_player;
    Racket* racket_AI;
    Gates* gates_player;
    Gates* gates_AI;
    Border* border;

    InputHandler* inputHandler;
};

#endif // PONGGAME_H