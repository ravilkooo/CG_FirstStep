#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>

class Triangle : public SceneNode
{
public:
    Triangle();
    Triangle(Vertex* vertices);

    void Update(float deltaTime);

    Vector3 GetCenterLocation() override;
};

#endif // TRIANGLE_H