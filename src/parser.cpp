#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Función para cargar los procesos desde el archivo
std::vector<Proceso> Parser::cargarProcesos(const std::string& ruta) {
    std::vector<Proceso> procesos;
    std::ifstream file(ruta);

    // Verificar si el archivo se abre correctamente
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return procesos;  // Regresar vacío si no se puede abrir el archivo
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid;
        int bt, at, prio;
        char coma;

        // Validación de la entrada de los datos
        if (ss >> pid >> coma >> bt >> coma >> at >> coma >> prio) {
            procesos.push_back({pid, bt, at, prio});
        } else {
            std::cerr << "Formato incorrecto en la línea: " << linea << std::endl;
        }
    }

    return procesos;
}

// Función para cargar los recursos desde el archivo
std::vector<Recurso> Parser::cargarRecursos(const std::string& ruta) {
    std::vector<Recurso> recursos;
    std::ifstream file(ruta);

    // Verificar si el archivo se abre correctamente
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return recursos;
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string nombre;
        int cantidad;

        // Validar y limpiar la entrada de los recursos
        if (std::getline(ss, nombre, ',') && ss >> cantidad) {
            recursos.emplace_back(nombre, cantidad);
        } else {
            std::cerr << "Formato incorrecto en la línea de recursos: " << linea << std::endl;
        }
    }

    return recursos;
}

// Función para cargar las acciones desde el archivo
std::vector<Accion> Parser::cargarAcciones(const std::string& ruta) {
    std::vector<Accion> acciones;
    std::ifstream file(ruta);

    // Verificar si el archivo se abre correctamente
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return acciones;
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid, tipoStr, recurso;
        int ciclo;
        char coma;

        // Validación de la entrada de acciones
        if (ss >> pid >> coma >> tipoStr >> coma >> recurso >> coma >> ciclo) {
            Accion acc = {pid, Accion::fromString(tipoStr), recurso, ciclo};
            acciones.push_back(acc);
        } else {
            std::cerr << "Formato incorrecto en la línea de acciones: " << linea << std::endl;
        }
    }

    return acciones;
}
