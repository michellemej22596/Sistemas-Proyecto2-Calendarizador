#include <iostream>
#include "parser.hpp"
#include "simulador.hpp"

int main() {
    auto procesos = Parser::cargarProcesos("../data/procesos.txt");
    auto recursos = Parser::cargarRecursos("../data/recursos.txt");
    auto acciones = Parser::cargarAcciones("../data/acciones.txt");

    ejecutarSimulacion(acciones, recursos, 20);  // 20 ciclos como máximo

    return 0;
}
