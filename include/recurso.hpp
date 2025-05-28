#pragma once
#include <string>
#include <mutex>
#include <condition_variable>

struct Recurso {
    std::string nombre;
    int contador; // cantidad inicial del recurso
    int disponible; // recursos disponibles actualmente

    // Mutex y condición para simular semáforos
    std::mutex mtx;
    std::condition_variable cv;

    Recurso(std::string nombre_, int cantidad_)
        : nombre(nombre_), contador(cantidad_), disponible(cantidad_) {}
};
