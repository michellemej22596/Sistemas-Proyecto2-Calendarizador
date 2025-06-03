#include <iostream>
#include "parser.hpp"
#include "simulador.hpp"
#include "sincronizacion.hpp"

int main() {
    auto procesos = Parser::cargarProcesos("../data/procesos.txt");

    std::cout << "=== Selecciona el algoritmo de planificación ===\n";
    std::cout << "1. FCFS\n";
    std::cout << "2. SJF\n";
    std::cout << "3. Round Robin\n";
    std::cout << "4. SRTF (Shortest Remaining Time First)\n";
    std::cout << "5. Priority\n";
    std::cout << "6. Simulación de sincronización (mutex y semáforo)\n";
    std::cout << "Opción: ";

    int opcion;
    std::cin >> opcion;

    switch (opcion) {
        case 1:
            simularFCFS(procesos, 30);
            break;
        case 2:
            simularSJF(procesos, 30);
            break;
        case 3: {
            int quantum;
            std::cout << "Ingresa el quantum para Round Robin: ";
            std::cin >> quantum;
            simularRoundRobin(procesos, quantum, 30);
            break;
        }
        case 4:
            simularSRTF(procesos, 30);
            break;
        case 5:
            simularPriority(procesos, 30);
            break;
        case 6: {
            // Carga dinámica para sincronización
            auto recursos = Parser::cargarRecursos("../data/recursos.txt");
            auto acciones = Parser::cargarAcciones("../data/acciones.txt");
            ejecutarSimulacion(acciones, recursos, 30);
            break;
        }
        default:
            std::cerr << "Opción inválida.\n";
            return 1;
    }

    return 0 ;
}