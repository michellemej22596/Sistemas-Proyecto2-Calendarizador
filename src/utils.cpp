#include "utils.hpp"
#include <fstream>

void guardarGanttCSV(const std::vector<EventoGantt>& eventos, const std::string& nombreArchivo) {
    std::ofstream file(nombreArchivo);
    file << "Proceso,Inicio,Fin\n";
    for (const auto& e : eventos) {
        file << e.pid << "," << e.inicio << "," << e.fin << "\n";
    }
    file.close();
}