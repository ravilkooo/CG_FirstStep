#include "PongPhysics.h"

PongPhysics::PongPhysics()
{
}

PongPhysics::PongPhysics(Scene* scene) : scene(scene)
{
}

PongPhysics::PongPhysics(Scene* scene, Ball* ball,
    Racket* racket_player, Racket* racket_AI,
    Gates* gates_player, Gates* gates_AI,
    Border* border)
    : scene(scene), ball(ball),
    racket_player(racket_player), racket_AI(racket_AI),
    gates_player(gates_player), gates_AI(gates_AI),
    border(border)
{
}

PongPhysics::~PongPhysics()
{
}

void PongPhysics::Update(float deltaTime)
{
    if (DirectX::XMVectorGetX(ball->velocity) > 0.f) {
        DirectX::XMFLOAT3 ball_loc;
        ball->GetCenterLocation(&ball_loc);
        DirectX::XMFLOAT3 AI_loc;
        racket_AI->GetCenterLocation(&AI_loc);
        racket_AI->velocity_magn_y = max(-AI_max_velocity, min((ball_loc.y - AI_loc.y) / deltaTime, AI_max_velocity));
    }
    else {
        racket_AI->velocity_magn_y = 0.f;
    }

    if (CheckBorderCollision(deltaTime))
    {
        border->HitBall(ball);
    }
    
    if (CheckRacketCollision(deltaTime, racket_player)) { //  && ball->direction_x < 0
        // player_bounce
        racket_player->HitBall(ball, deltaTime);
    }
    if (CheckRacketCollision(deltaTime, racket_AI)) { // && DirectX::XMVectorGetX(ball->velocity) > 0
        // AI_bounce
        racket_AI->HitBall(ball, deltaTime);
    }

    if (CheckGatesCollision(deltaTime, gates_player) && DirectX::XMVectorGetX(ball->velocity) < 0) {
        // player_lose
        AI_score++;
        std::cout << player_score << " : " << AI_score << std::endl;
        ball->Respawn();
    }
    if (CheckGatesCollision(deltaTime, gates_AI) && DirectX::XMVectorGetX(ball->velocity) > 0) {
        // AI_lose
        player_score++;
        std::cout << player_score << " : " << AI_score << std::endl;
        ball->Respawn();
    }


    for (auto node : scene->nodes)
    {
        // Обновление физики для каждого узла
        node->Update(deltaTime);
    }
}

bool PongPhysics::CheckBorderCollision(float deltaTime)
{
    DirectX::BoundingBox* borderBoxes = border->GetBoundingBoxes();
    DirectX::BoundingBox ballBox = ball->GetBoundingBox();
    DirectX::BoundingBox ballNextBox = ball->GetNextStepBoundingBox(deltaTime);
    // && borderBoxes[0].Intersects(ballNextBox) 
    // && borderBoxes[1].Intersects(ballNextBox) 
    return ((borderBoxes[0].Intersects(ballBox) && DirectX::XMVectorGetY(ball->velocity) > 0)
        || (borderBoxes[1].Intersects(ballBox) && DirectX::XMVectorGetY(ball->velocity) < 0));
}

bool PongPhysics::CheckGatesCollision(float deltaTime, Gates* gates)
{
    DirectX::BoundingBox gatesBox = gates->GetBoundingBox();
    DirectX::BoundingBox ballBox = ball->GetBoundingBox();
    DirectX::BoundingBox ballNextBox = ball->GetNextStepBoundingBox(deltaTime);
    return gatesBox.Intersects(ballBox) && gatesBox.Intersects(ballNextBox);
}

bool PongPhysics::CheckRacketCollision(float deltaTime, Racket* racket)
{
    DirectX::BoundingOrientedBox racketBox = racket->GetBoundingBox();
    DirectX::BoundingBox ballBox = ball->GetBoundingBox();
    DirectX::BoundingBox ballNextBox = ball->GetNextStepBoundingBox(deltaTime);
    return racketBox.Intersects(ballBox) && racketBox.Intersects(ballNextBox);
}

