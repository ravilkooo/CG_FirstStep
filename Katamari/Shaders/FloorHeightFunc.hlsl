#ifndef FLOOR_HIEGHT_FUNC
#define FLOOR_HIEGHT_FUNC

float FloorHeightFunc(float3 pos)
{
    return (2 * sin(pos.x * 0.5) * cos(pos.z * 0.5) + sin(pos.x * 10.0f) * 0.2) * 0.2;
}

#endif
