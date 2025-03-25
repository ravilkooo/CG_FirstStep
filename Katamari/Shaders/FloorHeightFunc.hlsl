#ifndef FLOOR_HIEGHT_FUNC
#define FLOOR_HIEGHT_FUNC

float FloorHeightFunc(float3 pos)
{
    return max(0, 2 * sin(pos.x * 0.5) * sin(pos.z * 0.5));
}

#endif
