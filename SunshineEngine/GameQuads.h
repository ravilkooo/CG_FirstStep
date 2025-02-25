#ifndef GAMEQUADS_H
#define GAMEQUADS_H

#include "Game.h"

#include "Quad.h"
#include "Triangle.h"

class GameQuads : public Game
{
public:
    GameQuads();
    ~GameQuads();

    void Run();

    void Initialize();
    void Update(float deltaTime);
    void Render();
};

#endif
