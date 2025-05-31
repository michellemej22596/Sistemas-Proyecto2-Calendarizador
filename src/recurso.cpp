#include "recurso.hpp"

// Función para que un proceso acceda al recurso
void Recurso::acceder() {
    std::unique_lock<std::mutex> lock(*mtx);  // Bloqueamos el mutex
    while (disponible == 0) {  // Si el recurso no está disponible
        cv->wait(lock);  // El proceso se bloquea y espera a que el recurso esté libre
    }
    disponible--;  // El proceso accede al recurso, disminuyendo la cantidad disponible
}

// Función para liberar el recurso
void Recurso::liberar() {
    std::lock_guard<std::mutex> lock(*mtx);  // Bloqueo del mutex, sin necesidad de esperar
    disponible++;  // El recurso se libera, aumentando su disponibilidad
    cv->notify_all();  // Notificamos a otros procesos que pueden acceder al recurso
}
