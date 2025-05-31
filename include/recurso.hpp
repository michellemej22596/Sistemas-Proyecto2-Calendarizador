#pragma once
#include <string>
#include <memory>  // Para std::shared_ptr
#include <mutex>
#include <condition_variable>

class Recurso {
public:
    std::string nombre;
    int contador;   // cantidad inicial del recurso
    int disponible; // recursos disponibles actualmente

    // Mutex y condición para simular semáforos
    std::shared_ptr<std::mutex> mtx;
    std::shared_ptr<std::condition_variable> cv;

    // Constructor
    Recurso(const std::string& nombre_, int cantidad_)
        : nombre(nombre_), contador(cantidad_), disponible(cantidad_) {
            mtx = std::make_shared<std::mutex>();
            cv = std::make_shared<std::condition_variable>();
        }

    // Funciones para acceder y liberar recursos
    void acceder();
    void liberar();
};
