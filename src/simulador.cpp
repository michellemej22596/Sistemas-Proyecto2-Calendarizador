#include "simulador.hpp"
#include "proceso.hpp"
#include <iostream>
#include <thread>
#include <map>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include "utils.hpp"


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

void simularFCFS(const std::vector<Proceso>& procesos, int ciclosMax) {
    std::vector<ProcesoFCFS> todos;
    for (const auto& p : procesos)
        todos.emplace_back(p);

    std::queue<ProcesoFCFS*> colaListos;
    ProcesoFCFS* ejecutando = nullptr;
    int ciclo = 0;

    std::vector<EventoGantt> eventosGantt;

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

                // Guardar evento de Gantt
                eventosGantt.push_back({
                    ejecutando->base.pid,
                    ejecutando->tiempoInicio,
                    ejecutando->tiempoFinal
                });

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

    // Guardar Gantt corregido
    guardarGanttCSV(eventosGantt, "gantt_fcfs.csv");
}

void simularSJF(const std::vector<Proceso>& procesosOriginal, int ciclosMax) {
    std::cout << "\n=== Simulación SJF ===\n";

    auto procesos = procesosOriginal;
    std::vector<Proceso> lista;
    std::vector<Proceso> completados;
    std::vector<EventoGantt> gantt;

    int ciclo = 0;
    bool ejecutando = false;
    Proceso actual;
    int tiempoRestante = 0;
    int inicioEjecucion = 0;

    while (ciclo <= ciclosMax) {
        for (const auto& p : procesos) {
            if (p.arrivalTime == ciclo) {
                std::cout << ">> Proceso " << p.pid << " ha llegado al ciclo " << ciclo << "\n";
                lista.push_back(p);
            }
        }

        if (!ejecutando && !lista.empty()) {
            std::sort(lista.begin(), lista.end(), [](const Proceso& a, const Proceso& b) {
                return a.burstTime < b.burstTime;
            });

            actual = lista.front();
            tiempoRestante = actual.burstTime;
            ejecutando = true;
            inicioEjecucion = ciclo;
            lista.erase(lista.begin());

            std::cout << ">> Proceso " << actual.pid << " inicia ejecución en ciclo " << ciclo << "\n";
        }

        if (ejecutando) {
            std::cout << "Ciclo " << ciclo << ": ejecutando " << actual.pid << "\n";
            tiempoRestante--;

            if (tiempoRestante == 0) {
                int fin = ciclo + 1;
                std::cout << ">> Proceso " << actual.pid << " finalizó en ciclo " << fin << "\n";

                // Agregar evento al diagrama de Gantt
                gantt.push_back({ actual.pid, inicioEjecucion, fin });

                Proceso terminado = actual;
                terminado.burstTime = fin - actual.arrivalTime;  // Duración real de ejecución
                completados.push_back(terminado);
                ejecutando = false;
            }
        } else {
            std::cout << "Ciclo " << ciclo << ": CPU ociosa\n";
        }

        ciclo++;
    }

    std::cout << "\n--- Métricas SJF ---\n";
    int totalWT = 0, totalTAT = 0;

    for (const auto& p : completados) {
        int tat = p.burstTime;
        int wt = tat - procesosOriginal[std::distance(procesosOriginal.begin(), std::find_if(procesosOriginal.begin(), procesosOriginal.end(), [&](const Proceso& original) {
            return original.pid == p.pid;
        }))].burstTime;
        totalTAT += tat;
        totalWT += wt;
        std::cout << p.pid << " - WT: " << wt << ", TAT: " << tat << "\n";
    }

    if (!completados.empty()) {
        std::cout << "\nPromedio WT: " << (float)totalWT / completados.size()
                  << ", Promedio TAT: " << (float)totalTAT / completados.size() << "\n";
    }

    guardarGanttCSV(gantt, "gantt_sjf.csv");
}

void simularRoundRobin(const std::vector<Proceso>& procesosOriginal, int quantum, int ciclosMax) {
    std::cout << "\n=== Simulación Round Robin (Quantum = " << quantum << ") ===\n";

    struct ProcesoRR {
        Proceso base;
        int tiempoRestante;
        int tiempoInicio = -1;
        int tiempoFinal = -1;

        ProcesoRR(const Proceso& p) : base(p), tiempoRestante(p.burstTime) {}
    };

    std::vector<ProcesoRR> procesos;
    for (const auto& p : procesosOriginal) {
        procesos.emplace_back(p);
    }

    std::queue<ProcesoRR*> colaListos;
    int ciclo = 0;
    int quantumRestante = quantum;
    ProcesoRR* ejecutando = nullptr;

    std::vector<EventoGantt> gantt;
    int inicioEjecucion = -1;

    while (ciclo <= ciclosMax) {
        for (auto& p : procesos) {
            if (p.base.arrivalTime == ciclo) {
                colaListos.push(&p);
                std::cout << ">> Proceso " << p.base.pid << " ha llegado al ciclo " << ciclo << "\n";
            }
        }

        if (!ejecutando && !colaListos.empty()) {
            ejecutando = colaListos.front();
            colaListos.pop();
            if (ejecutando->tiempoInicio == -1)
                ejecutando->tiempoInicio = ciclo;
            quantumRestante = std::min(quantum, ejecutando->tiempoRestante);
            inicioEjecucion = ciclo;
            std::cout << ">> Proceso " << ejecutando->base.pid << " inicia/reanuda ejecución en ciclo " << ciclo << "\n";
        }

        if (ejecutando) {
            std::cout << "Ciclo " << ciclo << ": ejecutando " << ejecutando->base.pid << "\n";
            ejecutando->tiempoRestante--;
            quantumRestante--;

            if (ejecutando->tiempoRestante == 0) {
                ejecutando->tiempoFinal = ciclo + 1;
                gantt.push_back({ ejecutando->base.pid, inicioEjecucion, ciclo + 1 });
                std::cout << ">> Proceso " << ejecutando->base.pid << " finalizó en ciclo " << ciclo + 1 << "\n";
                ejecutando = nullptr;
            } else if (quantumRestante == 0) {
                gantt.push_back({ ejecutando->base.pid, inicioEjecucion, ciclo + 1 });
                colaListos.push(ejecutando);
                ejecutando = nullptr;
            }
        } else {
            std::cout << "Ciclo " << ciclo << ": CPU ociosa\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ciclo++;
    }

    // Calcular métricas
    std::cout << "\n--- Métricas Round Robin ---\n";
    double totalWT = 0, totalTAT = 0;

    std::unordered_map<std::string, int> tiempoEjecutado;
    std::unordered_map<std::string, int> tiempoFinal;

    for (const auto& e : gantt) {
        tiempoEjecutado[e.pid] += (e.fin - e.inicio);
        tiempoFinal[e.pid] = std::max(tiempoFinal[e.pid], e.fin);
    }

    for (const auto& p : procesosOriginal) {
        int tat = tiempoFinal[p.pid] - p.arrivalTime;
        int wt = tat - p.burstTime;
        totalTAT += tat;
        totalWT += wt;
        std::cout << p.pid << " - WT: " << wt << ", TAT: " << tat << "\n";
    }

    std::cout << "\nPromedio WT: " << totalWT / procesosOriginal.size()
              << ", Promedio TAT: " << totalTAT / procesosOriginal.size() << "\n";

    guardarGanttCSV(gantt, "gantt_rr.csv");
}

void simularSRTF(const std::vector<Proceso>& procesosOriginal, int ciclosMax) {
    std::cout << "\n=== Simulación SRTF ===\n";

    struct ProcesoSRTF {
        Proceso base;
        int tiempoRestante;
        int tiempoInicio = -1;
        int tiempoFinal = -1;

        ProcesoSRTF(const Proceso& p) : base(p), tiempoRestante(p.burstTime) {}
    };

    std::vector<ProcesoSRTF> procesos;
    for (const auto& p : procesosOriginal)
        procesos.emplace_back(p);

    std::vector<ProcesoSRTF*> listos;
    ProcesoSRTF* ejecutando = nullptr;
    int ciclo = 0;
    int inicioEjecucion = -1;

    std::vector<EventoGantt> gantt;

    while (ciclo <= ciclosMax) {
        for (auto& p : procesos) {
            if (p.base.arrivalTime == ciclo) {
                listos.push_back(&p);
                std::cout << ">> Proceso " << p.base.pid << " ha llegado al ciclo " << ciclo << "\n";
            }
        }

        if (!listos.empty()) {
            std::sort(listos.begin(), listos.end(), [](ProcesoSRTF* a, ProcesoSRTF* b) {
                return a->tiempoRestante < b->tiempoRestante;
            });

            if (!ejecutando || ejecutando->tiempoRestante > listos.front()->tiempoRestante) {
                if (ejecutando && ejecutando->tiempoRestante > 0) {
                    gantt.push_back({ ejecutando->base.pid, inicioEjecucion, ciclo });
                    std::cout << ">> Proceso " << ejecutando->base.pid << " fue interrumpido en ciclo " << ciclo << "\n";
                    listos.push_back(ejecutando);
                }

                ejecutando = listos.front();
                listos.erase(listos.begin());

                if (ejecutando->tiempoInicio == -1)
                    ejecutando->tiempoInicio = ciclo;

                inicioEjecucion = ciclo;
                std::cout << ">> Proceso " << ejecutando->base.pid << " inicia/reanuda ejecución en ciclo " << ciclo << "\n";
            }
        }

        if (ejecutando) {
            std::cout << "Ciclo " << ciclo << ": ejecutando " << ejecutando->base.pid << "\n";
            ejecutando->tiempoRestante--;

            if (ejecutando->tiempoRestante == 0) {
                ejecutando->tiempoFinal = ciclo + 1;
                gantt.push_back({ ejecutando->base.pid, inicioEjecucion, ciclo + 1 });
                std::cout << ">> Proceso " << ejecutando->base.pid << " finalizó en ciclo " << ciclo + 1 << "\n";
                ejecutando = nullptr;
            }
        } else {
            std::cout << "Ciclo " << ciclo << ": CPU ociosa\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ciclo++;
    }

    std::cout << "\n--- Métricas SRTF ---\n";
    double totalWT = 0, totalTAT = 0;
    for (const auto& p : procesos) {
        int tat = p.tiempoFinal - p.base.arrivalTime;
        int wt = tat - p.base.burstTime;
        totalTAT += tat;
        totalWT += wt;
        std::cout << p.base.pid << " - WT: " << wt << ", TAT: " << tat << "\n";
    }

    std::cout << "\nPromedio WT: " << totalWT / procesos.size()
              << ", Promedio TAT: " << totalTAT / procesos.size() << "\n";

    guardarGanttCSV(gantt, "gantt_srtf.csv");
}

void simularPriority(const std::vector<Proceso>& procesosOriginal, int ciclosMax) {
    std::cout << "\n=== Simulación Priority ===\n";

    struct ProcesoPrioridad {
        Proceso base;
        int tiempoRestante;
        int tiempoInicio = -1;
        int tiempoFinal = -1;
        bool completado = false;

        ProcesoPrioridad(const Proceso& p) : base(p), tiempoRestante(p.burstTime) {}
    };

    std::vector<ProcesoPrioridad> procesos;
    for (const auto& p : procesosOriginal) {
        procesos.emplace_back(p);
    }

    std::vector<EventoGantt> gantt;
    int ciclo = 0;
    ProcesoPrioridad* ejecutando = nullptr;
    int inicioEjecucion = -1;

    while (ciclo <= ciclosMax) {
        for (auto& p : procesos) {
            if (p.base.arrivalTime == ciclo) {
                std::cout << ">> Proceso " << p.base.pid << " ha llegado al ciclo " << ciclo << "\n";
            }
        }

        if (!ejecutando) {
            std::vector<ProcesoPrioridad*> listos;
            for (auto& p : procesos) {
                if (!p.completado && p.base.arrivalTime <= ciclo)
                    listos.push_back(&p);
            }

            if (!listos.empty()) {
                std::sort(listos.begin(), listos.end(), [](ProcesoPrioridad* a, ProcesoPrioridad* b) {
                    return a->base.priority < b->base.priority;  // menor = mayor prioridad
                });

                ejecutando = listos.front();
                if (ejecutando->tiempoInicio == -1)
                    ejecutando->tiempoInicio = ciclo;
                inicioEjecucion = ciclo;

                std::cout << ">> Proceso " << ejecutando->base.pid << " inicia ejecución en ciclo " << ciclo << "\n";
            }
        }

        if (ejecutando) {
            std::cout << "Ciclo " << ciclo << ": ejecutando " << ejecutando->base.pid << "\n";
            ejecutando->tiempoRestante--;

            if (ejecutando->tiempoRestante == 0) {
                ejecutando->tiempoFinal = ciclo + 1;
                ejecutando->completado = true;
                gantt.push_back({ ejecutando->base.pid, inicioEjecucion, ciclo + 1 });
                std::cout << ">> Proceso " << ejecutando->base.pid << " finalizó en ciclo " << ciclo + 1 << "\n";
                ejecutando = nullptr;
            }
        } else {
            std::cout << "Ciclo " << ciclo << ": CPU ociosa\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ciclo++;
    }

    std::cout << "\n--- Métricas Priority ---\n";
    double totalWT = 0, totalTAT = 0;

    for (const auto& p : procesos) {
        int tat = p.tiempoFinal - p.base.arrivalTime;
        int wt = tat - p.base.burstTime;
        totalTAT += tat;
        totalWT += wt;
        std::cout << p.base.pid << " - WT: " << wt << ", TAT: " << tat << "\n";
    }

    std::cout << "\nPromedio WT: " << totalWT / procesos.size()
              << ", Promedio TAT: " << totalTAT / procesos.size() << "\n";

    guardarGanttCSV(gantt, "gantt_priority.csv");
}