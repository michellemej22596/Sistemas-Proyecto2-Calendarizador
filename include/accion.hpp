#pragma once
#include <string>
#include <stdexcept> 

enum class TipoAccion {
    READ,
    WRITE
};

struct Accion {
    std::string pid;
    TipoAccion tipo;
    std::string recurso;
    int ciclo;

    static TipoAccion fromString(const std::string& str) {
        if (str == "READ") return TipoAccion::READ;
        else if (str == "WRITE") return TipoAccion::WRITE;
        else throw std::invalid_argument("Tipo de acción inválido: " + str);
    }
};
