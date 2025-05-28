#pragma once
#include <string>

struct Proceso {
    std::string pid;
    int burstTime;
    int arrivalTime;
    int priority;
};
