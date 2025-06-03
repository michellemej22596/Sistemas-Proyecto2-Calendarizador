#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>  // para std::remove

// Función para cargar los procesos desde el archivo
std::vector<Proceso> Parser::cargarProcesos(const std::string& ruta) {
    std::vector<Proceso> procesos;
    std::ifstream file(ruta);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return procesos;
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid, bt_str, at_str, prio_str;

        if (std::getline(ss, pid, ',') &&
            std::getline(ss, bt_str, ',') &&
            std::getline(ss, at_str, ',') &&
            std::getline(ss, prio_str)) {
            
            // Quitar espacios
            pid.erase(remove(pid.begin(), pid.end(), ' '), pid.end());
            bt_str.erase(remove(bt_str.begin(), bt_str.end(), ' '), bt_str.end());
            at_str.erase(remove(at_str.begin(), at_str.end(), ' '), at_str.end());
            prio_str.erase(remove(prio_str.begin(), prio_str.end(), ' '), prio_str.end());

            procesos.push_back({
                pid,
                std::stoi(bt_str),
                std::stoi(at_str),
                std::stoi(prio_str)
            });
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

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return recursos;
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string nombre, cantidadStr;

        if (std::getline(ss, nombre, ',') &&
            std::getline(ss, cantidadStr)) {
            
            nombre.erase(remove(nombre.begin(), nombre.end(), ' '), nombre.end());
            cantidadStr.erase(remove(cantidadStr.begin(), cantidadStr.end(), ' '), cantidadStr.end());

            recursos.emplace_back(nombre, std::stoi(cantidadStr));
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

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ruta << std::endl;
        return acciones;
    }

    std::string linea;
    while (std::getline(file, linea)) {
        std::istringstream ss(linea);
        std::string pid, tipoStr, recurso, cicloStr;

        if (std::getline(ss, pid, ',') &&
            std::getline(ss, tipoStr, ',') &&
            std::getline(ss, recurso, ',') &&
            std::getline(ss, cicloStr)) {
            
            // Limpiar espacios
            pid.erase(remove(pid.begin(), pid.end(), ' '), pid.end());
            tipoStr.erase(remove(tipoStr.begin(), tipoStr.end(), ' '), tipoStr.end());
            recurso.erase(remove(recurso.begin(), recurso.end(), ' '), recurso.end());
            cicloStr.erase(remove(cicloStr.begin(), cicloStr.end(), ' '), cicloStr.end());

            Accion acc = {
                pid,
                Accion::fromString(tipoStr),
                recurso,
                std::stoi(cicloStr)
            };
            acciones.push_back(acc);
        } else {
            std::cerr << "Formato incorrecto en la línea de acciones: " << linea << std::endl;
        }
    }

    return acciones;
}
