# 🖥️ Simulador de Sistemas Operativos - UVG

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

Este proyecto es un simulador educativo de algoritmos de planificación y mecanismos de sincronización en sistemas operativos, desarrollado con C++ y visualizado mediante una interfaz gráfica usando **Dear ImGui** y **SFML**.

![image](https://github.com/user-attachments/assets/d1a3ecd5-75ce-42ee-b4cf-c30677b0a2d2)


## 🚀 Funcionalidades Principales

### Algoritmos de Planificación
- **FCFS** (First-Come First-Served)
- **SJF** (Shortest Job First)
- **SRTF** (Shortest Remaining Time First)
- **Round Robin** (con quantum configurable)
- **Priority Scheduling**

### Mecanismos de Sincronización
- Acceso mediante **Mutex**
- Acceso mediante **Semáforos**

### Visualización
- Diagrama de Gantt interactivo
- Métricas en tiempo real:
  - Tiempo de espera promedio
  - Turnaround time
  - Accesos a recursos

### Gestión de Datos
- Carga de archivos `.txt` para:
  - Procesos (`PID, BurstTime, ArrivalTime, Priority`)
  - Recursos (`nombre, cantidad`)
  - Acciones (`PID, tipo, recurso, ciclo`)

## 📂 Estructura del Proyecto

├── src/ # Código fuente
│ ├── mm.cpp # Interfaz gráfica
│ ├── simulador.cpp # Lógica de planificación
│ └── ...
├── data/ # Archivos de ejemplo
│ ├── procesos.txt
│ ├── recursos.txt
│ └── acciones.txt


## ⚙️ Instalación

Compilación Manual
```bash
g++ -I../include main.cpp parser.cpp simulador.cpp sincronizacion.cpp recurso.cpp utils.cpp -o simulador -pthread
```

🛠️ Uso del Simulador
Ejecutar el programa:
```bash
./simulador
```

📊 Resultados y Métricas
El simulador genera automáticamente:

Archivos CSV con resultados detallados

gantt_fcfs.csv, gantt_rr.csv, etc.

mutex_resultado.csv, semaforo_resultado.csv

Estadísticas de rendimiento

Visualización gráfica de la línea de tiempo
