#include <iostream>
#include "parser.hpp"
#include "simulador.hpp"

int main() {
    auto procesos = Parser::cargarProcesos("../data/procesos.txt");

    // Ejecutar el algoritmo FCFS con un máximo de 30 ciclos
    simularFCFS(procesos, 30);

    return 0;
}