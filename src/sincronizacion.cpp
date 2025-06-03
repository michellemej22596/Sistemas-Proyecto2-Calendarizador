#include "sincronizacion.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>

using namespace std::chrono;

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

    std::ofstream out("semaforo_resultado.csv");
    out << "Proceso,Inicio,Duracion\n";

    int contador = 0;
    std::mutex mtx_contador;
    auto start = steady_clock::now();

    auto hilo = [&](int id) {
        auto inicio = duration_cast<milliseconds>(steady_clock::now() - start).count();
        esperarRecurso(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        liberarRecurso(id);
        auto fin = duration_cast<milliseconds>(steady_clock::now() - start).count();
        {
            std::lock_guard<std::mutex> lock(mtx_contador);
            out << id << "," << inicio << "," << (fin - inicio) << "\n";
            contador++;
        }
    };

    std::thread t1(hilo, 1);
    std::thread t2(hilo, 2);
    std::thread t3(hilo, 3);

    t1.join();
    t2.join();
    t3.join();

    out.close();

    std::cout << "\n--- Resumen ---\n";
    std::cout << "Total de procesos que accedieron al recurso: " << contador << std::endl;
}

// ---------- Simulación con mutex ----------
void simularMutex() {
    std::cout << "\n=== Simulación de sincronización con mutex ===\n";

    std::ofstream out("mutex_resultado.csv");
    out << "Proceso,Inicio,Duracion\n";

    int contador = 0;
    std::mutex mtx_contador;
    auto start = steady_clock::now();

    auto hilo = [&](int id) {
        auto inicio = duration_cast<milliseconds>(steady_clock::now() - start).count();
        accederRecurso(id);
        auto fin = duration_cast<milliseconds>(steady_clock::now() - start).count();
        {
            std::lock_guard<std::mutex> lock(mtx_contador);
            out << id << "," << inicio << "," << (fin - inicio) << "\n";
            contador++;
        }
    };

    std::thread t1(hilo, 1);
    std::thread t2(hilo, 2);
    std::thread t3(hilo, 3);

    t1.join();
    t2.join();
    t3.join();

    out.close();

    std::cout << "\n--- Resumen ---\n";
    std::cout << "Total de procesos que accedieron al recurso: " << contador << std::endl;
}
