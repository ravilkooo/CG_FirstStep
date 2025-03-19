#ifndef GAMEQUADS_H
#define GAMEQUADS_H

#include "Game.h"

#include "Quad.h"
#include "Triangle.h"
#include "CommonVertex.h"

class GameQuads : public Game<CommonVertex>
{
public:
    GameQuads();
    ~GameQuads();

    void Run();

    void Update(float deltaTime);
    void Render();
};

#endif
