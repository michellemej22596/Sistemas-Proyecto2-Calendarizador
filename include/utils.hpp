#pragma once
#include <string>
#include <vector>

struct EventoGantt {
    std::string pid;
    int inicio;
    int fin;
};

// Declaración de la función:
void guardarGanttCSV(const std::vector<EventoGantt>& eventos, const std::string& nombreArchivo);