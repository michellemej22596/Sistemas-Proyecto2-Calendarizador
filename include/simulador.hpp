// simulador.hpp
#pragma once

#include "accion.hpp"
#include "recurso.hpp"
#include <vector>

void ejecutarSimulacion(const std::vector<Accion>& acciones, const std::vector<Recurso>& recursos, int ciclosMax = 20);
