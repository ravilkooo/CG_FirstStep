#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <chrono>

class GameTimer
{
public:
    GameTimer();

    void Tick();
    float GetDeltaTime() const;

private:
    std::chrono::high_resolution_clock::time_point lastTime;
    float deltaTime;
};

#endif // GAMETIMER_H