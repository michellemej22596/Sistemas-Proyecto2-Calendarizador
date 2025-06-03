// simulador.cpp
#include "simulador.hpp"
#include "proceso.hpp"
#include <iostream>
#include <thread>
#include <map>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <algorithm>

// -------------------- MÉTRICAS Y ESTRUCTURAS --------------------

struct ProcesoFCFS {
    Proceso base;
    int tiempoInicio = -1;
    int tiempoFinal = -1;
    int tiempoRestante;

    ProcesoFCFS(Proceso p)
        : base(p), tiempoRestante(p.burstTime) {}
};

// -------------------- FUNCIONES EXISTENTES --------------------

std::map<std::string, Recurso> recursosGlobales;
int totalAcciones = 0;
int totalReads = 0;
int totalWrites = 0;
std::unordered_map<std::string, int> accesosPorProceso;

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

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    recurso.liberar();

    std::cout << "[LIBERADO] Proceso " << accion.pid << " liberó " << accion.recurso << std::endl;

    totalAcciones++;
    if (accion.tipo == TipoAccion::READ) totalReads++;
    else if (accion.tipo == TipoAccion::WRITE) totalWrites++;
    accesosPorProceso[accion.pid]++;
}

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

    std::cout << "\n--- Resumen de métricas ---" << std::endl;
    std::cout << "Total de acciones ejecutadas: " << totalAcciones << std::endl;
    std::cout << "Acciones de lectura (READ): " << totalReads << std::endl;
    std::cout << "Acciones de escritura (WRITE): " << totalWrites << std::endl;

    std::cout << "\nAccesos por proceso:" << std::endl;
    for (const auto& [pid, count] : accesosPorProceso) {
        std::cout << " - " << pid << ": " << count << " accesos" << std::endl;
    }

    std::cout << "\n=== Simulación finalizada ===" << std::endl;
}

// -------------------- NUEVA FUNCIÓN: FCFS --------------------

void simularFCFS(const std::vector<Proceso>& procesos, int ciclosMax = 30) {
    std::vector<ProcesoFCFS> todos;
    for (const auto& p : procesos)
        todos.emplace_back(p);

    std::queue<ProcesoFCFS*> colaListos;
    ProcesoFCFS* ejecutando = nullptr;
    int ciclo = 0;

    std::cout << "\n=== Simulación FCFS ===\n";

    while (ciclo <= ciclosMax) {
        for (auto& p : todos) {
            if (p.base.arrivalTime == ciclo) {
                colaListos.push(&p);
                std::cout << ">> Proceso " << p.base.pid << " ha llegado al ciclo " << ciclo << "\n";
            }
        }

        if (!ejecutando && !colaListos.empty()) {
            ejecutando = colaListos.front();
            colaListos.pop();
            ejecutando->tiempoInicio = ciclo;
            std::cout << ">> Proceso " << ejecutando->base.pid << " inicia ejecución en ciclo " << ciclo << "\n";
        }

        if (ejecutando) {
            std::cout << "Ciclo " << ciclo << ": ejecutando " << ejecutando->base.pid << "\n";
            ejecutando->tiempoRestante--;

            if (ejecutando->tiempoRestante == 0) {
                ejecutando->tiempoFinal = ciclo + 1;
                std::cout << ">> Proceso " << ejecutando->base.pid << " finalizó en ciclo " << ciclo + 1 << "\n";
                ejecutando = nullptr;
            }
        } else {
            std::cout << "Ciclo " << ciclo << ": CPU ociosa\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ciclo++;
    }

    std::cout << "\n--- Métricas FCFS ---\n";
    double totalWT = 0, totalTAT = 0;
    for (const auto& p : todos) {
        int tat = p.tiempoFinal - p.base.arrivalTime;
        int wt = p.tiempoInicio - p.base.arrivalTime;
        totalTAT += tat;
        totalWT += wt;
        std::cout << p.base.pid << " - WT: " << wt << ", TAT: " << tat << "\n";
    }

    std::cout << "\nPromedio WT: " << totalWT / todos.size()
              << ", Promedio TAT: " << totalTAT / todos.size() << "\n";
}