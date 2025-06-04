#include <iostream>
#include "../include/parser.hpp"
#include "../include/simulador.hpp"
#include "../include/sincronizacion.hpp"
#include "../include/gui.hpp"

int main() {
    std::cout << "=== SIMULADOR DE SISTEMAS OPERATIVOS ===" << std::endl;
    std::cout << "Universidad del Valle de Guatemala" << std::endl;
    std::cout << "Sistemas Operativos - Proyecto 2" << std::endl;
    std::cout << "\n🎯 Seleccione el modo de ejecución:" << std::endl;
    std::cout << "1. 🎨 Interfaz gráfica SFML (RECOMENDADO)" << std::endl;
    std::cout << "2. 🖥️  Modo consola (básico)" << std::endl;
    std::cout << "Opción: ";
    
    int modo;
    std::cin >> modo;
    
    if (modo == 1) {
        std::cout << "\n🚀 Iniciando interfaz gráfica SFML..." << std::endl;
        std::cout << "📋 Funcionalidades disponibles:" << std::endl;
        std::cout << "   • Simulación de Calendarización (FCFS, SJF, RR, SRTF, Priority)" << std::endl;
        std::cout << "   • Simulación de Sincronización (Mutex y Semáforos)" << std::endl;
        std::cout << "   • Timeline gráfico interactivo con scroll" << std::endl;
        std::cout << "   • Métricas de eficiencia en tiempo real" << std::endl;
        std::cout << "   • Carga de datos desde archivos" << std::endl;
        std::cout << "   • Exportación de resultados" << std::endl;
        std::cout << "\n⌨️  Controles:" << std::endl;
        std::cout << "   • ESC = Volver al menú principal" << std::endl;
        std::cout << "   • SPACE = Iniciar/Pausar simulación" << std::endl;
        std::cout << "   • S = Detener simulación" << std::endl;
        std::cout << "   • R = Reiniciar simulación" << std::endl;
        std::cout << "   • C = Limpiar datos" << std::endl;
        std::cout << "   • 1/2 = Acceso rápido a simulaciones" << std::endl;
        std::cout << "   • Rueda del mouse = Scroll en timeline" << std::endl;
        
        try {
            InterfazSFML interfaz;
            interfaz.ejecutar();
        } catch (const std::exception& e) {
            std::cerr << "\n❌ Error en la interfaz gráfica: " << e.what() << std::endl;
            std::cout << "🔄 Cambiando automáticamente a modo consola..." << std::endl;
            modo = 2; // Cambiar a modo consola
        }
    }
    
    if (modo == 2) {
        std::cout << "\n🖥️  Modo consola activado" << std::endl;
        
        try {
            // Intentar cargar procesos desde diferentes rutas
            std::vector<Proceso> procesos;
            std::vector<std::string> rutas = {"data/procesos.txt", "../data/procesos.txt", "./data/procesos.txt"};
            
            bool procesosCargados = false;
            for (const auto& ruta : rutas) {
                try {
                    procesos = Parser::cargarProcesos(ruta);
                    if (!procesos.empty()) {
                        std::cout << "✅ Procesos cargados desde: " << ruta << std::endl;
                        procesosCargados = true;
                        break;
                    }
                } catch (...) {
                    continue;
                }
            }
            
            if (!procesosCargados) {
                std::cout << "⚠️  No se pudieron cargar procesos, usando datos de ejemplo..." << std::endl;
                procesos = {
                    {"P1", 8, 0, 2},
                    {"P2", 4, 1, 1},
                    {"P3", 9, 2, 3},
                    {"P4", 5, 3, 2}
                };
            }

            std::cout << "\n=== Selecciona el algoritmo de planificación ===\n";
            std::cout << "1. FCFS (First Come First Served)\n";
            std::cout << "2. SJF (Shortest Job First)\n";
            std::cout << "3. Round Robin\n";
            std::cout << "4. SRTF (Shortest Remaining Time First)\n";
            std::cout << "5. Priority Scheduling\n";
            std::cout << "6. Simulación de sincronización (mutex y semáforo)\n";
            std::cout << "Opción: ";

            int opcion;
            std::cin >> opcion;

            switch (opcion) {
                case 1:
                    std::cout << "🔄 Ejecutando FCFS..." << std::endl;
                    simularFCFS(procesos, 30);
                    break;
                case 2:
                    std::cout << "🔄 Ejecutando SJF..." << std::endl;
                    simularSJF(procesos, 30);
                    break;
                case 3: {
                    int quantum;
                    std::cout << "Ingresa el quantum para Round Robin (1-20): ";
                    std::cin >> quantum;
                    if (quantum < 1) quantum = 1;
                    if (quantum > 20) quantum = 20;
                    std::cout << "🔄 Ejecutando Round Robin con quantum " << quantum << "..." << std::endl;
                    simularRoundRobin(procesos, quantum, 30);
                    break;
                }
                case 4:
                    std::cout << "🔄 Ejecutando SRTF..." << std::endl;
                    simularSRTF(procesos, 30);
                    break;
                case 5:
                    std::cout << "🔄 Ejecutando Priority..." << std::endl;
                    simularPriority(procesos, 30);
                    break;
                case 6: {
                    std::cout << "=== Selecciona el tipo de sincronización ===\n";
                    std::cout << "1. 🔒 Mutex (Exclusión mutua)\n";
                    std::cout << "2. 🔢 Semáforo (Contador de recursos)\n";
                    std::cout << "Opción: ";

                    int tipoSync;
                    std::cin >> tipoSync;

                    if (tipoSync == 1) {
                        std::cout << "🔒 Ejecutando simulación Mutex..." << std::endl;
                        simularMutex();
                    } else if (tipoSync == 2) {
                        std::cout << "🔢 Ejecutando simulación Semáforo..." << std::endl;
                        simularSemaforo();
                    } else {
                        std::cerr << "❌ Opción de sincronización inválida.\n";
                        return 1;
                    }
                    break;
                }
                default:
                    std::cerr << "❌ Opción inválida.\n";
                    return 1;
            }
            
            std::cout << "\n✅ Simulación completada exitosamente!" << std::endl;
            std::cout << "📊 Revise los archivos CSV generados para ver los resultados detallados." << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Error durante la simulación: " << e.what() << std::endl;
            return 1;
        }
    }

    std::cout << "\n👋 ¡Gracias por usar el simulador!" << std::endl;
    std::cout << "🎓 Universidad del Valle de Guatemala - Sistemas Operativos" << std::endl;
    return 0;
}
