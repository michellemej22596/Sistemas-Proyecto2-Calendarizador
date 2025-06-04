#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>
#include <queue>
#include <algorithm>
#include <map>
#include <unordered_map>

// Dear ImGui includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Tus headers existentes - ahora desde include/
#include "../include/parser.hpp"
#include "../include/simulador.hpp"
#include "../include/sincronizacion.hpp"
#include "../include/utils.hpp"
#include "../include/proceso.hpp"
#include "../include/accion.hpp"
#include "../include/recurso.hpp"

// Estructura para eventos de UI en tiempo real
struct EventoUI {
    std::string pid;
    int ciclo;
    std::string estado; // "EJECUTANDO", "ESPERANDO", "TERMINADO", "ACCESED", "WAITING"
    std::string recurso = "";
    
    EventoUI(std::string p, int c, std::string e, std::string r = "") 
        : pid(p), ciclo(c), estado(e), recurso(r) {}
};

class SimuladorUI {
private:
    // Estado de la UI
    int tipoSimulacion = 0; // 0 = Scheduling, 1 = Sincronización
    int algoritmoSeleccionado = 0;
    int tipoSincronizacion = 0;
    int quantum = 2;
    int ciclosMax = 30;
    
    // Archivos
    char archivoProcesos[256] = "data/procesos.txt";
    char archivoRecursos[256] = "data/recursos.txt";
    char archivoAcciones[256] = "data/acciones.txt";
    
    // Estado de simulación
    std::atomic<bool> simulacionActiva{false};
    std::atomic<bool> simulacionPausada{false};
    std::atomic<int> cicloActual{0};
    
    // Datos cargados
    std::vector<Proceso> procesosActuales;
    std::vector<Recurso> recursosActuales;
    std::vector<Accion> accionesActuales;
    
    // Visualización en tiempo real
    std::vector<EventoUI> eventosUI;
    std::mutex mutexEventos;
    
    // Métricas en tiempo real
    std::atomic<double> avgWaitingTime{0.0};
    std::atomic<double> avgTurnaroundTime{0.0};
    std::atomic<int> procesosCompletados{0};
    
    // Colores para procesos
    std::vector<ImU32> coloresProcesos = {
        IM_COL32(255, 100, 100, 255), // Rojo
        IM_COL32(100, 255, 100, 255), // Verde
        IM_COL32(100, 100, 255, 255), // Azul
        IM_COL32(255, 255, 100, 255), // Amarillo
        IM_COL32(255, 100, 255, 255), // Magenta
        IM_COL32(100, 255, 255, 255), // Cian
        IM_COL32(255, 150, 100, 255), // Naranja
        IM_COL32(150, 100, 255, 255), // Púrpura
    };

public:
    void renderUI() {
        // Ventana principal
        ImGui::Begin("Simulador de Sistemas Operativos - UVG", nullptr, 
                    ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
        
        renderMenuBar();
        renderConfiguracion();
        renderVisualizacion();
        renderControles();
        renderMetricas();
        
        ImGui::End();
        
        // Ventanas adicionales
        renderVentanaInfo();
        renderVentanaArchivos();
    }

private:
    void renderMenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Archivo")) {
                if (ImGui::MenuItem("Cargar Procesos", "Ctrl+P")) {
                    cargarProcesos();
                }
                if (ImGui::MenuItem("Cargar Recursos", "Ctrl+R")) {
                    cargarRecursos();
                }
                if (ImGui::MenuItem("Cargar Acciones", "Ctrl+A")) {
                    cargarAcciones();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Limpiar Todo", "Ctrl+L")) {
                    limpiarTodo();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Simulación")) {
                if (ImGui::MenuItem("Iniciar", "Space", false, !simulacionActiva)) {
                    iniciarSimulacion();
                }
                if (ImGui::MenuItem("Pausar", "P", false, simulacionActiva && !simulacionPausada)) {
                    pausarSimulacion();
                }
                if (ImGui::MenuItem("Reanudar", "R", false, simulacionActiva && simulacionPausada)) {
                    reanudarSimulacion();
                }
                if (ImGui::MenuItem("Detener", "S", false, simulacionActiva)) {
                    detenerSimulacion();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void renderConfiguracion() {
        ImGui::Separator();
        ImGui::Text("Configuración de Simulación");
        ImGui::Separator();
        
        // Tipo de simulación
        ImGui::RadioButton("Algoritmos de Calendarización", &tipoSimulacion, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Mecanismos de Sincronización", &tipoSimulacion, 1);
        
        ImGui::Spacing();
        
        if (tipoSimulacion == 0) {
            renderConfigScheduling();
        } else {
            renderConfigSincronizacion();
        }
        
        // Configuración general
        ImGui::Spacing();
        ImGui::SliderInt("Ciclos Máximos", &ciclosMax, 10, 100);
    }

    void renderConfigScheduling() {
        const char* algoritmos[] = {"FCFS", "SJF", "Round Robin", "SRTF", "Priority"};
        ImGui::Combo("Algoritmo de Scheduling", &algoritmoSeleccionado, algoritmos, 5);
        
        if (algoritmoSeleccionado == 2) { // Round Robin
            ImGui::SliderInt("Quantum", &quantum, 1, 10);
        }
        
        ImGui::InputText("Archivo de Procesos", archivoProcesos, sizeof(archivoProcesos));
        ImGui::SameLine();
        if (ImGui::Button("Cargar##procesos")) {
            cargarProcesos();
        }
    }

    void renderConfigSincronizacion() {
        const char* tipos[] = {"Mutex", "Semáforo"};
        ImGui::Combo("Tipo de Sincronización", &tipoSincronizacion, tipos, 2);
        
        ImGui::InputText("Procesos##sync", archivoProcesos, sizeof(archivoProcesos));
        ImGui::SameLine();
        if (ImGui::Button("Cargar##proc_sync")) {
            cargarProcesos();
        }
        
        ImGui::InputText("Recursos", archivoRecursos, sizeof(archivoRecursos));
        ImGui::SameLine();
        if (ImGui::Button("Cargar##recursos")) {
            cargarRecursos();
        }
        
        ImGui::InputText("Acciones", archivoAcciones, sizeof(archivoAcciones));
        ImGui::SameLine();
        if (ImGui::Button("Cargar##acciones")) {
            cargarAcciones();
        }
    }

    void renderVisualizacion() {
        ImGui::Separator();
        ImGui::Text("Visualización Dinámica");
        ImGui::Separator();
        
        // Información del ciclo actual
        ImGui::Text("Ciclo Actual: %d / %d", cicloActual.load(), ciclosMax);
        if (simulacionActiva) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "EJECUTANDO");
            if (simulacionPausada) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "(PAUSADO)");
            }
        }
        
        // Diagrama de Gantt dinámico
        ImGui::BeginChild("DiagramaGantt", ImVec2(0, 200), true, 
                         ImGuiWindowFlags_HorizontalScrollbar);
        
        dibujarDiagramaGantt();
        
        ImGui::EndChild();
        
        // Leyenda de colores
        renderLeyendaProcesos();
    }

    void dibujarDiagramaGantt() {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        
        if (canvasSize.x < 50) return;
        
        float blockWidth = 40.0f;
        float blockHeight = 30.0f;
        float yOffset = 20.0f;
        
        // Línea de tiempo
        drawList->AddLine(
            ImVec2(canvasPos.x, canvasPos.y + yOffset + blockHeight + 10),
            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + yOffset + blockHeight + 10),
            IM_COL32(128, 128, 128, 255), 2.0f
        );
        
        // Dibujar eventos
        std::lock_guard<std::mutex> lock(mutexEventos);
        
        std::map<std::string, int> procesoIndex;
        int nextIndex = 0;
        
        for (const auto& evento : eventosUI) {
            if (procesoIndex.find(evento.pid) == procesoIndex.end()) {
                procesoIndex[evento.pid] = nextIndex++;
            }
            
            float x = canvasPos.x + evento.ciclo * blockWidth;
            float y = canvasPos.y + yOffset;
            
            // Color basado en el estado
            ImU32 color;
            if (evento.estado == "EJECUTANDO") {
                color = coloresProcesos[procesoIndex[evento.pid] % coloresProcesos.size()];
            } else if (evento.estado == "ESPERANDO" || evento.estado == "WAITING") {
                color = IM_COL32(200, 200, 200, 255); // Gris
            } else if (evento.estado == "ACCESED") {
                color = IM_COL32(0, 200, 0, 255); // Verde brillante
            } else {
                color = IM_COL32(150, 150, 150, 255);
            }
            
            // Dibujar bloque
            ImVec2 rectMin(x, y);
            ImVec2 rectMax(x + blockWidth - 2, y + blockHeight);
            
            drawList->AddRectFilled(rectMin, rectMax, color);
            drawList->AddRect(rectMin, rectMax, IM_COL32(0, 0, 0, 255));
            
            // Texto del proceso
            std::string texto = evento.pid;
            if (!evento.recurso.empty()) {
                texto += "\n" + evento.recurso;
            }
            
            ImVec2 textSize = ImGui::CalcTextSize(texto.c_str());
            ImVec2 textPos(x + (blockWidth - textSize.x) * 0.5f, y + (blockHeight - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(0, 0, 0, 255), texto.c_str());
        }
        
        // Marcadores de ciclo
        for (int i = 0; i <= cicloActual.load() && i <= ciclosMax; i += 5) {
            float x = canvasPos.x + i * blockWidth;
            drawList->AddLine(
                ImVec2(x, canvasPos.y + yOffset + blockHeight + 5),
                ImVec2(x, canvasPos.y + yOffset + blockHeight + 15),
                IM_COL32(100, 100, 100, 255)
            );
            
            std::string cicloText = std::to_string(i);
            ImVec2 textPos(x - 5, canvasPos.y + yOffset + blockHeight + 20);
            drawList->AddText(textPos, IM_COL32(100, 100, 100, 255), cicloText.c_str());
        }
        
        // Hacer scroll automático al ciclo actual
        if (simulacionActiva && !simulacionPausada) {
            float scrollTarget = cicloActual.load() * blockWidth - canvasSize.x * 0.8f;
            if (scrollTarget > 0) {
                ImGui::SetScrollX(scrollTarget);
            }
        }
        
        // Expandir el área de contenido
        ImGui::Dummy(ImVec2(std::max(ciclosMax * blockWidth, canvasSize.x), blockHeight + 50));
    }

    void renderLeyendaProcesos() {
        if (procesosActuales.empty()) return;
        
        ImGui::Text("Leyenda de Procesos:");
        ImGui::Columns(std::min(4, (int)procesosActuales.size()), "LeyendaColumns", false);
        
        for (size_t i = 0; i < procesosActuales.size(); ++i) {
            ImU32 color = coloresProcesos[i % coloresProcesos.size()];
            
            // Cuadrito de color
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            drawList->AddRectFilled(pos, ImVec2(pos.x + 15, pos.y + 15), color);
            drawList->AddRect(pos, ImVec2(pos.x + 15, pos.y + 15), IM_COL32(0, 0, 0, 255));
            
            ImGui::Dummy(ImVec2(20, 15));
            ImGui::SameLine();
            ImGui::Text("%s", procesosActuales[i].pid.c_str());
            
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void renderControles() {
        ImGui::Separator();
        ImGui::Text("Controles de Simulación");
        ImGui::Separator();
        
        if (!simulacionActiva) {
            if (ImGui::Button("Iniciar Simulación", ImVec2(150, 30))) {
                iniciarSimulacion();
            }
        } else {
            if (!simulacionPausada) {
                if (ImGui::Button("Pausar", ImVec2(100, 30))) {
                    pausarSimulacion();
                }
            } else {
                if (ImGui::Button("Reanudar", ImVec2(100, 30))) {
                    reanudarSimulacion();
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Detener", ImVec2(100, 30))) {
                detenerSimulacion();
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reiniciar", ImVec2(100, 30))) {
            reiniciarSimulacion();
        }
    }

    void renderMetricas() {
        ImGui::Separator();
        ImGui::Text("Métricas en Tiempo Real");
        ImGui::Separator();
        
        ImGui::Columns(3, "MetricasColumns", true);
        
        ImGui::Text("Avg Waiting Time");
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f", avgWaitingTime.load());
        
        ImGui::NextColumn();
        ImGui::Text("Avg Turnaround Time");
        ImGui::TextColored(ImVec4(0, 0, 1, 1), "%.2f", avgTurnaroundTime.load());
        
        ImGui::NextColumn();
        ImGui::Text("Procesos Completados");
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "%d / %zu", 
                          procesosCompletados.load(), procesosActuales.size());
        
        ImGui::Columns(1);
    }

    void renderVentanaInfo() {
        ImGui::Begin("Información de Procesos y Recursos");
        
        // Usar pestañas simples en lugar de TabBar
        static int tabActiva = 0;
        
        if (ImGui::Button("Procesos")) tabActiva = 0;
        if (tipoSimulacion == 1) {
            ImGui::SameLine();
            if (ImGui::Button("Recursos")) tabActiva = 1;
            ImGui::SameLine();
            if (ImGui::Button("Acciones")) tabActiva = 2;
        }
        
        ImGui::Separator();
        
        switch (tabActiva) {
            case 0:
                renderTablaProcesos();
                break;
            case 1:
                if (tipoSimulacion == 1) renderTablaRecursos();
                break;
            case 2:
                if (tipoSimulacion == 1) renderTablaAcciones();
                break;
        }
        
        ImGui::End();
    }

    void renderTablaProcesos() {
        if (procesosActuales.empty()) {
            ImGui::Text("No hay procesos cargados.");
            return;
        }
        
        // Usar columnas simples en lugar de BeginTable
        ImGui::Columns(4, "ProcesosColumns", true);
        ImGui::Text("PID"); ImGui::NextColumn();
        ImGui::Text("Burst Time"); ImGui::NextColumn();
        ImGui::Text("Arrival Time"); ImGui::NextColumn();
        ImGui::Text("Priority"); ImGui::NextColumn();
        ImGui::Separator();
        
        for (const auto& proceso : procesosActuales) {
            ImGui::Text("%s", proceso.pid.c_str()); ImGui::NextColumn();
            ImGui::Text("%d", proceso.burstTime); ImGui::NextColumn();
            ImGui::Text("%d", proceso.arrivalTime); ImGui::NextColumn();
            ImGui::Text("%d", proceso.priority); ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void renderTablaRecursos() {
        if (recursosActuales.empty()) {
            ImGui::Text("No hay recursos cargados.");
            return;
        }
        
        ImGui::Columns(2, "RecursosColumns", true);
        ImGui::Text("Nombre"); ImGui::NextColumn();
        ImGui::Text("Cantidad"); ImGui::NextColumn();
        ImGui::Separator();
        
        for (const auto& recurso : recursosActuales) {
            ImGui::Text("%s", recurso.nombre.c_str()); ImGui::NextColumn();
            ImGui::Text("%d", recurso.disponible); ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void renderTablaAcciones() {
        if (accionesActuales.empty()) {
            ImGui::Text("No hay acciones cargadas.");
            return;
        }
        
        ImGui::Columns(4, "AccionesColumns", true);
        ImGui::Text("PID"); ImGui::NextColumn();
        ImGui::Text("Acción"); ImGui::NextColumn();
        ImGui::Text("Recurso"); ImGui::NextColumn();
        ImGui::Text("Ciclo"); ImGui::NextColumn();
        ImGui::Separator();
        
        for (const auto& accion : accionesActuales) {
            ImGui::Text("%s", accion.pid.c_str()); ImGui::NextColumn();
            ImGui::Text("%s", accion.tipo == TipoAccion::READ ? "READ" : "WRITE"); ImGui::NextColumn();
            ImGui::Text("%s", accion.recurso.c_str()); ImGui::NextColumn();
            ImGui::Text("%d", accion.ciclo); ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void renderVentanaArchivos() {
        ImGui::Begin("Gestión de Archivos");
        
        ImGui::Text("Archivos de configuración:");
        ImGui::Separator();
        
        ImGui::Text("Procesos: %s", archivoProcesos);
        ImGui::Text("Recursos: %s", archivoRecursos);
        ImGui::Text("Acciones: %s", archivoAcciones);
        
        ImGui::Separator();
        
        if (ImGui::Button("Recargar Todos los Archivos")) {
            cargarProcesos();
            if (tipoSimulacion == 1) {
                cargarRecursos();
                cargarAcciones();
            }
        }
        
        ImGui::End();
    }

    // Funciones de callback para actualizar UI
    void agregarEventoUI(const std::string& pid, int ciclo, const std::string& estado, const std::string& recurso = "") {
        std::lock_guard<std::mutex> lock(mutexEventos);
        eventosUI.emplace_back(pid, ciclo, estado, recurso);
    }

    void actualizarMetricas(double wt, double tat, int completados) {
        avgWaitingTime.store(wt);
        avgTurnaroundTime.store(tat);
        procesosCompletados.store(completados);
    }

    // Funciones de carga de archivos
    void cargarProcesos() {
        try {
            procesosActuales = Parser::cargarProcesos(archivoProcesos);
            std::cout << "Procesos cargados: " << procesosActuales.size() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error cargando procesos: " << e.what() << std::endl;
        }
    }

    void cargarRecursos() {
        try {
            recursosActuales = Parser::cargarRecursos(archivoRecursos);
            std::cout << "Recursos cargados: " << recursosActuales.size() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error cargando recursos: " << e.what() << std::endl;
        }
    }

    void cargarAcciones() {
        try {
            accionesActuales = Parser::cargarAcciones(archivoAcciones);
            std::cout << "Acciones cargadas: " << accionesActuales.size() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error cargando acciones: " << e.what() << std::endl;
        }
    }

    // Control de simulación
    void iniciarSimulacion() {
        if (procesosActuales.empty()) {
            cargarProcesos();
        }
        
        if (procesosActuales.empty()) {
            std::cerr << "No hay procesos para simular" << std::endl;
            return;
        }
        
        simulacionActiva = true;
        simulacionPausada = false;
        cicloActual = 0;
        
        // Limpiar eventos anteriores
        {
            std::lock_guard<std::mutex> lock(mutexEventos);
            eventosUI.clear();
        }
        
        // Ejecutar simulación en hilo separado
        std::thread([this]() {
            if (tipoSimulacion == 0) {
                ejecutarSimulacionScheduling();
            } else {
                ejecutarSimulacionSincronizacion();
            }
            simulacionActiva = false;
        }).detach();
    }

    void ejecutarSimulacionScheduling() {
        // Crear callbacks para actualizar UI
        auto callbackEvento = [this](const std::string& pid, int ciclo, const std::string& estado) {
            this->agregarEventoUI(pid, ciclo, estado);
            this->cicloActual = ciclo;
        };
        
        auto callbackMetricas = [this](double wt, double tat, int completados) {
            this->actualizarMetricas(wt, tat, completados);
        };
        
        // Llamar a las funciones de simulación modificadas
        switch (algoritmoSeleccionado) {
            case 0: 
                simularFCFS_UI(procesosActuales, ciclosMax, callbackEvento, callbackMetricas);
                break;
            case 1: 
                simularSJF_UI(procesosActuales, ciclosMax, callbackEvento, callbackMetricas);
                break;
            case 2: 
                simularRoundRobin_UI(procesosActuales, quantum, ciclosMax, callbackEvento, callbackMetricas);
                break;
            case 3: 
                simularSRTF_UI(procesosActuales, ciclosMax, callbackEvento, callbackMetricas);
                break;
            case 4: 
                simularPriority_UI(procesosActuales, ciclosMax, callbackEvento, callbackMetricas);
                break;
        }
    }

    void ejecutarSimulacionSincronizacion() {
        if (recursosActuales.empty() || accionesActuales.empty()) {
            cargarRecursos();
            cargarAcciones();
        }
        
        auto callbackEvento = [this](const std::string& pid, int ciclo, const std::string& estado, const std::string& recurso) {
            this->agregarEventoUI(pid, ciclo, estado, recurso);
            this->cicloActual = ciclo;
        };
        
        if (tipoSincronizacion == 0) {
            simularMutex_UI(callbackEvento);
        } else {
            simularSemaforo_UI(callbackEvento);
        }
    }

    void pausarSimulacion() {
        simulacionPausada = true;
    }

    void reanudarSimulacion() {
        simulacionPausada = false;
    }

    void detenerSimulacion() {
        simulacionActiva = false;
        simulacionPausada = false;
    }

    void reiniciarSimulacion() {
        detenerSimulacion();
        cicloActual = 0;
        {
            std::lock_guard<std::mutex> lock(mutexEventos);
            eventosUI.clear();
        }
        avgWaitingTime = 0.0;
        avgTurnaroundTime = 0.0;
        procesosCompletados = 0;
    }

    void limpiarTodo() {
        detenerSimulacion();
        procesosActuales.clear();
        recursosActuales.clear();
        accionesActuales.clear();
        reiniciarSimulacion();
    }

    // ==================== FUNCIONES DE SIMULACIÓN UI ====================
    // (Las mismas funciones de simulación que antes...)

    void simularFCFS_UI(const std::vector<Proceso>& procesos, int ciclosMax,
                       std::function<void(const std::string&, int, const std::string&)> callback,
                       std::function<void(double, double, int)> metricsCallback) {
        
        struct ProcesoFCFS {
            Proceso base;
            int tiempoInicio = -1;
            int tiempoFinal = -1;
            int tiempoRestante;
            ProcesoFCFS(Proceso p) : base(p), tiempoRestante(p.burstTime) {}
        };

        std::vector<ProcesoFCFS> todos;
        for (const auto& p : procesos)
            todos.emplace_back(p);

        std::queue<ProcesoFCFS*> colaListos;
        ProcesoFCFS* ejecutando = nullptr;
        int ciclo = 0;

        while (ciclo <= ciclosMax && simulacionActiva) {
            while (simulacionPausada && simulacionActiva) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            if (!simulacionActiva) break;

            for (auto& p : todos) {
                if (p.base.arrivalTime == ciclo) {
                    colaListos.push(&p);
                    callback(p.base.pid, ciclo, "LLEGADA");
                }
            }

            if (!ejecutando && !colaListos.empty()) {
                ejecutando = colaListos.front();
                colaListos.pop();
                ejecutando->tiempoInicio = ciclo;
            }

            if (ejecutando) {
                callback(ejecutando->base.pid, ciclo, "EJECUTANDO");
                ejecutando->tiempoRestante--;

                if (ejecutando->tiempoRestante == 0) {
                    ejecutando->tiempoFinal = ciclo + 1;
                    callback(ejecutando->base.pid, ciclo, "TERMINADO");
                    ejecutando = nullptr;
                }
            }

            double totalWT = 0, totalTAT = 0;
            int completados = 0;
            for (const auto& p : todos) {
                if (p.tiempoFinal > 0) {
                    int tat = p.tiempoFinal - p.base.arrivalTime;
                    int wt = p.tiempoInicio - p.base.arrivalTime;
                    totalTAT += tat;
                    totalWT += wt;
                    completados++;
                }
            }
            
            if (completados > 0) {
                metricsCallback(totalWT / completados, totalTAT / completados, completados);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            ciclo++;
        }
    }

    // Implementar las otras funciones de simulación de manera similar...
    void simularSJF_UI(const std::vector<Proceso>& procesos, int ciclosMax,
                      std::function<void(const std::string&, int, const std::string&)> callback,
                      std::function<void(double, double, int)> metricsCallback) {
        // Implementación similar a FCFS pero con lógica SJF
    }

    void simularRoundRobin_UI(const std::vector<Proceso>& procesos, int quantum, int ciclosMax,
                             std::function<void(const std::string&, int, const std::string&)> callback,
                             std::function<void(double, double, int)> metricsCallback) {
        // Implementación Round Robin
    }

    void simularSRTF_UI(const std::vector<Proceso>& procesos, int ciclosMax,
                       std::function<void(const std::string&, int, const std::string&)> callback,
                       std::function<void(double, double, int)> metricsCallback) {
        // Implementación SRTF
    }

    void simularPriority_UI(const std::vector<Proceso>& procesos, int ciclosMax,
                           std::function<void(const std::string&, int, const std::string&)> callback,
                           std::function<void(double, double, int)> metricsCallback) {
        // Implementación Priority
    }

    void simularMutex_UI(std::function<void(const std::string&, int, const std::string&, const std::string&)> callback) {
        // Implementación Mutex
    }

    void simularSemaforo_UI(std::function<void(const std::string&, int, const std::string&, const std::string&)> callback) {
        // Implementación Semáforo
    }
};

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error inicializando GLFW" << std::endl;
        return -1;
    }

    // Configurar OpenGL para Mac
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1400, 900, 
                                         "Simulador de Sistemas Operativos - UVG", 
                                         nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Error inicializando GLEW" << std::endl;
        return -1;
    }

    // Configurar Dear ImGui (SIN docking)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // REMOVIDO: io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.4f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.5f, 0.7f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    SimuladorUI simulador;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        simulador.renderUI();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
