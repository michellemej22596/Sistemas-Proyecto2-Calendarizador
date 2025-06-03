// simulador.cpp
#include "simulador.hpp"
#include <iostream>
#include <thread>
#include <map>
#include <chrono>
#include <unordered_map>

// Mapeo global de recursos por nombre
std::map<std::string, Recurso> recursosGlobales;

// Métricas globales
int totalAcciones = 0;
int totalReads = 0;
int totalWrites = 0;
std::unordered_map<std::string, int> accesosPorProceso;

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

    // Registrar métricas
    totalAcciones++;
    if (accion.tipo == TipoAccion::READ) totalReads++;
    else if (accion.tipo == TipoAccion::WRITE) totalWrites++;
    accesosPorProceso[accion.pid]++;
}

// Simulador principal por ciclo
void ejecutarSimulacion(const std::vector<Accion>& acciones, const std::vector<Recurso>& recursos, int ciclosMax) {
    for (const auto& r : recursos) {
        recursosGlobales.emplace(r.nombre, r);
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

    // Mostrar métricas al final
    std::cout << "\n--- Resumen de métricas ---" << std::endl;
    std::cout << "Total de acciones ejecutadas: " << totalAcciones << std::endl;
    std::cout << "Acciones de lectura (READ): " << totalReads << std::endl;
    std::cout << "Acciones de escritura (WRITE): " << totalWrites << std::endl;

    std::cout << "\nAccesos por proceso:" << std::endl;
    for (const auto& [pid, count] : accesosPorProceso) {
        std::cout << " - " << pid << ": " << count << " accesos" << std::endl;
    }

    std::cout << "\n=== Simulación finalizada ==="  << std::endl;
}