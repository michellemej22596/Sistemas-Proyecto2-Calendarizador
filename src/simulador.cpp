// simulador.cpp
#include "simulador.hpp"
#include <iostream>
#include <thread>
#include <map>
#include <chrono>

// Mapeo global de recursos por nombre
std::map<std::string, Recurso> recursosGlobales;

// Función para ejecutar una acción
void ejecutarAccion(const Accion& accion) {
    auto it = recursosGlobales.find(accion.recurso);
    if (it == recursosGlobales.end()) {
        std::cerr << "[ERROR] Recurso no encontrado: " << accion.recurso << std::endl;
        return;
    }
    Recurso& recurso = it->second;

    std::cout << "Proceso " << accion.pid << " intentando "
              << (accion.tipo == TipoAccion::READ ? "leer" : "escribir")
              << " " << accion.recurso << " en ciclo " << accion.ciclo << std::endl;

    recurso.acceder();

    std::cout << "[ACCESED] Proceso " << accion.pid << " accedió a "
              << accion.recurso << " ("
              << (accion.tipo == TipoAccion::READ ? "READ" : "WRITE") << ")" << std::endl;

    // Simulación de duración de la acción (1 ciclo = 500 ms)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    recurso.liberar();

    std::cout << "[LIBERADO] Proceso " << accion.pid << " liberó " << accion.recurso << std::endl;
}

// Simulador principal por ciclo
void ejecutarSimulacion(const std::vector<Accion>& acciones, const std::vector<Recurso>& recursos, int ciclosMax) {
    for (const auto& r : recursos) {
        recursosGlobales.emplace(r.nombre, r);  // Inserción segura sin constructor por defecto
    }

    std::cout << "\n=== Iniciando simulación ===" << std::endl;

    for (int ciclo = 0; ciclo <= ciclosMax; ciclo++) {
        std::cout << "\n--- Ciclo " << ciclo << " ---" << std::endl;

        std::vector<std::thread> hilos;

        for (const auto& accion : acciones) {
            if (accion.ciclo == ciclo) {
                hilos.emplace_back(ejecutarAccion, accion);
            }
        }

        for (auto& h : hilos) {
            if (h.joinable()) h.join();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n=== Simulación finalizada ===" << std::endl;
}
