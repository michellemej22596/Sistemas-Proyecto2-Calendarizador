#pragma once

#include "accion.hpp"
#include "recurso.hpp"
#include "proceso.hpp"
#include <vector>

void ejecutarSimulacion(const std::vector<Accion>& acciones, const std::vector<Recurso>& recursos, int ciclosMax = 20);
void simularFCFS(const std::vector<Proceso>& procesos, int ciclosMax = 30);
void simularSJF(const std::vector<Proceso>& procesos, int ciclosMax = 30);
void simularRoundRobin(const std::vector<Proceso>& procesos, int quantum = 2, int ciclosMax = 30);
void simularSRTF(const std::vector<Proceso>& procesos, int ciclosMax = 30);