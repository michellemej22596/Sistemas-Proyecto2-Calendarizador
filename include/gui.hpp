
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <functional>
#include <ctime>

// Incluir headers del proyecto
#include "proceso.hpp"
#include "recurso.hpp"
#include "accion.hpp"
#include "parser.hpp"
#include "simulador.hpp"
#include "sincronizacion.hpp"
#include "utils.hpp"

// Estructura para el estado de la simulación
struct EstadoSimulacion {
    bool enEjecucion = false;
    bool pausada = false;
    int cicloActual = 0;
    int ciclosMaximos = 30;
    std::string algoritmoActual = "";
    std::set<std::string> algoritmosSeleccionados;
    std::string modoSincronizacion = "";
    int quantum = 2;
    std::vector<EventoGantt> eventosGantt;
    std::map<std::string, sf::Color> coloresProcesos;
    float velocidadSimulacion = 1.0f;
};

// Estructura para métricas
struct Metricas {
    double tiempoEsperaPromedio = 0.0;
    double tiempoRetornoPromedio = 0.0;
    double utilizacionCPU = 0.0;
    int procesosTotales = 0;
    int procesosCompletados = 0;
};

// Estructura para botones mejorada
struct Boton {
    sf::RectangleShape forma;
    sf::Text texto;
    bool activo = true;
    bool presionado = false;
    bool seleccionado = false;
    std::function<void()> callback;
    
    Boton(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& textoBoton, sf::Font& fuente);
    void dibujar(sf::RenderWindow& ventana);
    bool contienePunto(sf::Vector2f punto);
    void manejarClick();
};

// Estructura para botones simples (sin fuente externa)
struct BotonSimple {
    sf::RectangleShape forma;
    sf::Text texto;
    std::function<void()> callback;
    sf::Vector2f posicion;
    sf::Vector2f tamaño;
    bool seleccionado = false;
    
    bool contienePunto(sf::Vector2f punto) {
        return forma.getGlobalBounds().contains(punto);
    }
    
    void manejarClick() {
        if (callback) {
            callback();
        }
    }
    
    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(forma);
        ventana.draw(texto);
    }
};

// Enumeración para las pantallas
enum class Pantalla {
    MENU_PRINCIPAL,
    CALENDARIZACION,
    SINCRONIZACION,
    CONFIGURACION,
    RESULTADOS,
    CARGA_ARCHIVOS,
    INFORMACION
};

class InterfazSFML {
private:
    // Ventana y recursos gráficos
    sf::RenderWindow ventana;
    sf::Font fuente;
    sf::Font fuenteTitulo;
    
    // Estado de la aplicación
    Pantalla pantallaActual;
    EstadoSimulacion estado;
    Metricas metricas;
    
    // Datos del sistema
    std::vector<Proceso> procesos;
    std::vector<Recurso> recursos;
    std::vector<Accion> acciones;
    
    // Elementos de UI
    std::vector<std::unique_ptr<Boton>> botones;
    std::vector<std::unique_ptr<BotonSimple>> botonesSimples;
    
    // Configuración de ventana
    const int ANCHO_VENTANA = 1400;
    const int ALTO_VENTANA = 900;
    
    // Colores del tema
    sf::Color colorFondo = sf::Color(240, 248, 255);
    sf::Color colorPrimario = sf::Color(70, 130, 180);
    sf::Color colorSecundario = sf::Color(100, 149, 237);
    sf::Color colorExito = sf::Color(34, 139, 34);
    sf::Color colorError = sf::Color(220, 20, 60);
    sf::Color colorAdvertencia = sf::Color(255, 165, 0);
    
    // Variables para animaciones y scroll
    sf::Clock relojAnimacion;
    float offsetScrollX = 0.0f;
    float escalaX = 1.0f;
    
    // Variables para entrada de texto
    std::string textoEntrada = "";
    bool entradaActiva = false;
    sf::RectangleShape cajaTexto;
    sf::Text textoMostrado;

public:
    InterfazSFML();
    ~InterfazSFML();
    
    // Métodos principales
    bool inicializar();
    void ejecutar();
    void manejarEventos();
    void actualizar();
    void dibujar();
    
    // Manejo de eventos específicos
    void manejarTeclado(const sf::Event& evento);
    void manejarClickTimeline(sf::Vector2f posicion);
    void manejarScrollTimeline(float delta);
    
    // Manejo de pantallas
    void cambiarPantalla(Pantalla nuevaPantalla);
    void dibujarMenuPrincipal();
    void dibujarPantallaCalendarizacion();
    void dibujarPantallaSincronizacion();
    void dibujarPantallaConfiguracion();
    void dibujarPantallaResultados();
    void dibujarPantallaCargaArchivos();
    void dibujarPantallaInformacion();
    
    // Métodos de dibujo simplificados
    void dibujarTextoSimple(const std::string& texto, sf::Vector2f posicion, sf::Color color = sf::Color::Black, int tamaño = 16);
    void dibujarRectangulo(sf::Vector2f posicion, sf::Vector2f tamaño, sf::Color color, sf::Color borde = sf::Color::Transparent);
    void crearBotonSimple(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& texto, 
                         std::function<void()> callback, bool seleccionado = false);
    
    // Paneles específicos (stubs)
    void dibujarPanelEstado();
    void dibujarPanelAlgoritmos();
    void dibujarPanelConfiguracion();
    void dibujarPanelMetricas();
    void dibujarPanelRecursosAcciones();
    void dibujarControlesSimulacion();
    void dibujarControlesCalendarizacion();
    
    // Timeline y visualización (stubs)
    void dibujarTimelineGantt();
    void dibujarTimelineSincronizacion();
    void dibujarEscalaTiempo(sf::Vector2f pos, float ancho);
    void dibujarLineaTiempoProceso(const Proceso& proceso, sf::Vector2f pos, float ancho, float altura);
    void dibujarAccionEnTimeline(const Accion& accion, sf::Vector2f pos, sf::Vector2f tam);
    
    // Tablas de información (stubs)
    void dibujarTablaProcesos();
    void dibujarTablaRecursos();
    void dibujarTablaAcciones();
    
    // Control de simulación
    void toggleAlgoritmo(const std::string& algoritmo);
    void seleccionarAlgoritmo(const std::string& algoritmo);
    void iniciarSimulacionCalendarizacion();
    void iniciarSimulacionSincronizacion();
    void pausarSimulacion();
    void detenerSimulacion();
    void reiniciarSimulacion();
    void seleccionarModoSincronizacion(const std::string& modo);
    void configurarQuantum(int nuevoQuantum);
    
    // Ejecución de algoritmos (stubs)
    void ejecutarAlgoritmo(const std::string& algoritmo);
    void cargarEventosGantt(const std::string& algoritmo);
    
    // Carga y manejo de datos
    void cargarDatosIniciales();
    void cargarArchivoProcesos();
    void cargarArchivoRecursos();
    void cargarArchivoAcciones();
    void limpiarDatos();
    void guardarResultados();
    void mostrarDialogoCarga(const std::string& tipo);
    
    // Utilidades de UI (stubs)
    void crearBoton(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& texto, std::function<void()> callback);
    void limpiarBotones();
    void dibujarEncabezado(const std::string& titulo, sf::Vector2f posicion);
    void dibujarTexto(const std::string& texto, sf::Vector2f posicion, sf::Color color = sf::Color::Black, int tamaño = 16);
    void dibujarBarraProgreso(sf::Vector2f posicion, sf::Vector2f tamaño, float progreso, sf::Color color);
    
    // Manejo de entrada (stubs)
    void manejarEntradaTexto(sf::Event& evento);
    void activarEntradaTexto(sf::Vector2f posicion, sf::Vector2f tamaño);
    void desactivarEntradaTexto();
    
    // Animaciones y efectos (stubs)
    void actualizarAnimaciones();
    void animarTransicion();
    sf::Color interpolarColor(sf::Color inicio, sf::Color fin, float t);
    
    // Utilidades
    void asignarColoresProcesos();
    sf::Color obtenerColorProceso(const std::string& pid);
    void calcularMetricas();
    void mostrarMensaje(const std::string& mensaje, sf::Color color = sf::Color::Black);
};

// Alias para compatibilidad
using InterfazGrafica = InterfazSFML;
