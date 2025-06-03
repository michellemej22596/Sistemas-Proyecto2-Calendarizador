#include "sincronizacion.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Mutex global para acceso general
std::mutex mtx;

// ---------- Funciones usando mutex ----------
void accederRecurso(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "[Mutex] Proceso " << id << " accede al recurso." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

// ---------- Semáforo simulado ----------
std::mutex mtx_sem;
std::condition_variable cv_sem;
bool recursoDisponible = true;

void esperarRecurso(int id) {
    std::unique_lock<std::mutex> lock(mtx_sem);
    cv_sem.wait(lock, [] { return recursoDisponible; });  // Espera activa

    recursoDisponible = false;
    std::cout << "[Semáforo] Proceso " << id << " accede al recurso." << std::endl;
}

void liberarRecurso(int id) {
    std::lock_guard<std::mutex> lock(mtx_sem);
    std::cout << "[Semáforo] Proceso " << id << " libera el recurso." << std::endl;
    recursoDisponible = true;
    cv_sem.notify_one();
}

// ---------- Simulación con semáforo ----------
void simularSemaforo() {
    std::cout << "\n=== Simulación de sincronización con semáforo ===\n";

    int contador = 0;
    std::mutex mtx_contador;

    std::thread t1([&]() {
        esperarRecurso(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        liberarRecurso(1);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    std::thread t2([&]() {
        esperarRecurso(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        liberarRecurso(2);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    std::thread t3([&]() {
        esperarRecurso(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        liberarRecurso(3);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    t1.join();
    t2.join();
    t3.join();

    std::cout << "\n--- Resumen ---\n";
    std::cout << "Total de procesos que accedieron al recurso: " << contador << std::endl;
}

// ---------- Simulación con mutex ----------
void simularMutex() {
    std::cout << "\n=== Simulación de sincronización con mutex ===\n";

    int contador = 0;
    std::mutex mtx_contador;

    std::thread t1([&]() {
        accederRecurso(1);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    std::thread t2([&]() {
        accederRecurso(2);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    std::thread t3([&]() {
        accederRecurso(3);
        std::lock_guard<std::mutex> lock(mtx_contador);
        contador++;
    });

    t1.join();
    t2.join();
    t3.join();

    std::cout << "\n--- Resumen ---\n";
    std::cout << "Total de procesos que accedieron al recurso: " << contador << std::endl;
}