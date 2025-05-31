#include "../include/parser.hpp"
#include <iostream> 

int main() {
    auto procesos = Parser::cargarProcesos("data/procesos.txt");
    auto recursos = Parser::cargarRecursos("data/recursos.txt");
    auto acciones = Parser::cargarAcciones("data/acciones.txt");

    for (const auto& p : procesos)
        std::cout << p.pid << " - " << p.burstTime << "\n";

    return 0;
}
