#pragma once
#include <mutex>

void accederRecurso(int id);               // Función para acceder a un recurso con Mutex
void esperarRecurso(int id);                // Función para esperar el acceso a un recurso (Semáforo)
void liberarRecurso();                     // Función para liberar un recurso (Semáforo)
