#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Proceso> Parser::cargarProcesos(const std::string& ruta) {
    std::vector<Proceso> procesos;
    std::ifstream file(ruta);
    std::string linea;

    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid;
        int bt, at, prio;
        char coma;

        if (ss >> pid >> coma >> bt >> coma >> at >> coma >> prio) {
            procesos.push_back({pid, bt, at, prio});
        }
    }

    return procesos;
}

std::vector<Recurso> Parser::cargarRecursos(const std::string& ruta) {
    std::vector<Recurso> recursos;
    std::ifstream file(ruta);
    std::string linea;

    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string nombre;
        int cantidad;
        char coma;

        if (ss >> nombre >> coma >> cantidad) {
            recursos.emplace_back(nombre, cantidad);
        }
    }

    return recursos;
}

std::vector<Accion> Parser::cargarAcciones(const std::string& ruta) {
    std::vector<Accion> acciones;
    std::ifstream file(ruta);
    std::string linea;

    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid, tipoStr, recurso;
        int ciclo;
        char coma;

        if (ss >> pid >> coma >> tipoStr >> coma >> recurso >> coma >> ciclo) {
            Accion acc = {pid, Accion::fromString(tipoStr), recurso, ciclo};
            acciones.push_back(acc);
        }
    }

    return acciones;
}
