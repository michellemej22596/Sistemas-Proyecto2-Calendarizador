#include <iostream>
#include <thread>
#include "sincronizacion.hpp"

int main() {
    std::cout << "Bienvenido al simulador!" << std::endl;

    // Crear hilos para simular la ejecución de procesos
    std::thread t1(accederRecurso, 1);
    std::thread t2(accederRecurso, 2);
    std::thread t3(accederRecurso, 3);

    // Esperar a que todos los hilos terminen
    t1.join();
    t2.join();
    t3.join();

    // Proceso usando semáforo
    std::thread t4(esperarRecurso, 4);
    std::thread t5(esperarRecurso, 5);

    // Liberar el recurso después de un tiempo
    std::this_thread::sleep_for(std::chrono::seconds(1));
    liberarRecurso();  // Liberamos el recurso

    t4.join();
    t5.join();

    return 0;
}
