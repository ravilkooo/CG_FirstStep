#ifndef PONG_PHYSICS_H
#define PONG_PHYSICS_H

#include <PhysicsEngine.h>

#include "Ball.h"
#include "Racket.h"
#include "Gates.h"
#include "Border.h"

class PongPhysics : public PhysicsEngine
{
public:
    PongPhysics();
    PongPhysics(Scene* scene);
    PongPhysics(Scene* scene, Ball* ball,
        Racket* racket_player, Racket* racket_AI,
        Gates* gates_player, Gates* gates_AI,
        Border* border);
    ~PongPhysics();

    void Update(float deltaTime);

    bool CheckBorderCollision(float deltaTime);
    bool CheckGatesCollision(float deltaTime, Gates* gates);
    bool CheckRacketCollision(float deltaTime, Racket* racket);

    Scene* scene;

    Ball* ball;
    Racket* racket_player;
    Racket* racket_AI;
    Gates* gates_player;
    Gates* gates_AI;
    Border* border;

    float AI_max_velocity = 3.f;
};

#endif // !PONG_PHYSICS_H