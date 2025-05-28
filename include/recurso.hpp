#pragma once
#include <string>
#include <mutex>
#include <condition_variable>

struct Recurso {
    std::string nombre;
    int contador;
    std::mutex mutex;
    std::condition_variable cv;
    int disponible;
};
