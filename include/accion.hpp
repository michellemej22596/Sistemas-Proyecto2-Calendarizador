#pragma once
#include <string>

struct Accion {
    std::string pid;
    std::string tipo;     // "READ" o "WRITE"
    std::string recurso;
    int ciclo;
};
