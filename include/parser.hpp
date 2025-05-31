#pragma once
#include <vector>
#include <string>
#include "proceso.hpp"
#include "recurso.hpp"
#include "accion.hpp"

namespace Parser {
    std::vector<Proceso> cargarProcesos(const std::string& ruta);
    std::vector<Recurso> cargarRecursos(const std::string& ruta);
    std::vector<Accion> cargarAcciones(const std::string& ruta);
}
