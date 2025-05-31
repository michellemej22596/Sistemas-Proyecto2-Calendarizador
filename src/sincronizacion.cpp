#include "sincronizacion.hpp"
#include <iostream>
#include <thread>  
#include <mutex>
#include <condition_variable>

// Mutex global para el acceso a recursos
std::mutex mtx;

// Función para acceder al recurso con Mutex
void accederRecurso(int id) {
    std::lock_guard<std::mutex> lock(mtx);  // Bloqueo automático
    std::cout << "Proceso " << id << " está accediendo al recurso." << std::endl;
}

// Semáforo simulado con mutex y condition_variable
std::mutex mtx_sem;
std::condition_variable cv_sem;
bool recursoDisponible = true;

// Función para esperar el recurso (simulando semáforo)
void esperarRecurso(int id) {
    std::unique_lock<std::mutex> lock(mtx_sem);
    while (!recursoDisponible) {
        cv_sem.wait(lock);  // Espera hasta que el recurso esté disponible
    }
    recursoDisponible = false;  // Bloquea el recurso
    std::cout << "Proceso " << id << " accede al recurso." << std::endl;
}

// Función para liberar el recurso (simulando semáforo)
void liberarRecurso() {
    std::lock_guard<std::mutex> lock(mtx_sem);
    recursoDisponible = true;  // Libera el recurso
    cv_sem.notify_one();  // Notifica a un proceso esperando
}
