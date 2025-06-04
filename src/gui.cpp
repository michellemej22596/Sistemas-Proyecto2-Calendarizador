#include "../include/gui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// Constructor del botón - SIMPLIFICADO para debugging
Boton::Boton(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& textoBoton, sf::Font& fuente) 
{
    std::cout << "🔧 Creando botón: " << textoBoton << std::endl;
    
    forma.setPosition(posicion);
    forma.setSize(tamaño);
    forma.setFillColor(sf::Color(70, 130, 180));
    forma.setOutlineThickness(2);
    forma.setOutlineColor(sf::Color(50, 100, 150));
    
    // Configurar texto con debugging
    texto.setString(textoBoton);
    texto.setFont(fuente);
    texto.setCharacterSize(18);
    texto.setFillColor(sf::Color::White);
    texto.setStyle(sf::Text::Bold);
    
    // Centrar texto en el botón
    sf::FloatRect textBounds = texto.getLocalBounds();
    std::cout << "📏 Bounds del texto: " << textBounds.width << "x" << textBounds.height << std::endl;
    
    texto.setPosition(sf::Vector2f(
        posicion.x + (tamaño.x - textBounds.width) / 2,
        posicion.y + (tamaño.y - textBounds.height) / 2 - 5
    ));
    
    std::cout << "✅ Botón creado en posición: " << posicion.x << "," << posicion.y << std::endl;
}

void Boton::dibujar(sf::RenderWindow& ventana) {
    // Cambiar color según estado
    if (!activo) {
        forma.setFillColor(sf::Color(150, 150, 150));
        texto.setFillColor(sf::Color(80, 80, 80));
    } else if (presionado) {
        forma.setFillColor(sf::Color(50, 100, 150));
        texto.setFillColor(sf::Color::Yellow);
    } else if (seleccionado) {
        forma.setFillColor(sf::Color(34, 139, 34));
        texto.setFillColor(sf::Color::White);
    } else {
        forma.setFillColor(sf::Color(70, 130, 180));
        texto.setFillColor(sf::Color::White);
    }
    
    // Dibujar forma y texto
    ventana.draw(forma);
    ventana.draw(texto);
}

bool Boton::contienePunto(sf::Vector2f punto) {
    return forma.getGlobalBounds().contains(punto);
}

void Boton::manejarClick() {
    if (callback && activo) {
        std::cout << "🖱️ Botón clickeado: " << texto.getString().toAnsiString() << std::endl;
        callback();
    }
}

// Constructor de InterfazSFML - SIMPLIFICADO
InterfazSFML::InterfazSFML() : pantallaActual(Pantalla::MENU_PRINCIPAL) {
    std::cout << "🏗️ Construyendo InterfazSFML..." << std::endl;
    ventana.create(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Simulador de Sistemas Operativos - UVG");
    ventana.setFramerateLimit(60);
    std::cout << "✅ Ventana creada: " << ANCHO_VENTANA << "x" << ALTO_VENTANA << std::endl;
}

InterfazSFML::~InterfazSFML() {
    if (ventana.isOpen()) {
        ventana.close();
    }
}

bool InterfazSFML::inicializar() {
    std::cout << "🔧 Inicializando interfaz SFML..." << std::endl;
    
    // NUEVO ENFOQUE: Usar fuente por defecto de SFML
    std::cout << "🔤 Usando fuente por defecto del sistema..." << std::endl;
    
    // Crear un texto de prueba para verificar que funciona
    sf::Text textoPrueba;
    textoPrueba.setString("TEXTO DE PRUEBA");
    textoPrueba.setCharacterSize(24);
    textoPrueba.setFillColor(sf::Color::Red);
    
    std::cout << "✅ Usando fuente por defecto de SFML" << std::endl;
    
    // Inicializar texto mostrado
    textoMostrado.setString("Texto inicial");
    textoMostrado.setCharacterSize(16);
    textoMostrado.setFillColor(sf::Color::Black);
    
    // Cargar datos iniciales
    cargarDatosIniciales();
    asignarColoresProcesos();
    
    std::cout << "✅ Interfaz SFML inicializada correctamente" << std::endl;
    return true;
}

void InterfazSFML::cargarDatosIniciales() {
    std::cout << "📂 Cargando datos iniciales..." << std::endl;
    
    // Usar datos de ejemplo siempre para simplificar
    procesos = {
        {"P1", 8, 0, 2},
        {"P2", 4, 1, 1},
        {"P3", 9, 2, 3},
        {"P4", 5, 3, 2}
    };
    
    recursos = {
        Recurso("CPU", 1),
        Recurso("Memory", 512),
        Recurso("Disk", 2)
    };
    
    // CORREGIDO: Crear acciones una por una para evitar problemas de inicialización
    acciones.clear();
    acciones.push_back({"P1", TipoAccion::READ, "CPU", 0});
    acciones.push_back({"P2", TipoAccion::WRITE, "Memory", 1});
    acciones.push_back({"P3", TipoAccion::read, "Disk", 2});
    acciones.push_back({"P4", TipoAccion::WRITE, "CPU", 3});
    
    std::cout << "📊 Datos cargados: " << procesos.size() << " procesos, " 
              << recursos.size() << " recursos, " << acciones.size() << " acciones" << std::endl;
}

void InterfazSFML::ejecutar() {
    if (!inicializar()) {
        std::cerr << "❌ Error inicializando la interfaz" << std::endl;
        return;
    }
    
    std::cout << "🚀 Interfaz SFML ejecutándose..." << std::endl;
    
    while (ventana.isOpen()) {
        manejarEventos();
        actualizar();
        dibujar();
    }
}

void InterfazSFML::manejarEventos() {
    sf::Event evento;
    while (ventana.pollEvent(evento)) {
        switch (evento.type) {
            case sf::Event::Closed:
                std::cout << "👋 Cerrando aplicación..." << std::endl;
                ventana.close();
                break;
                
            case sf::Event::MouseButtonPressed:
                if (evento.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f posicionMouse = sf::Vector2f(
                        static_cast<float>(evento.mouseButton.x), 
                        static_cast<float>(evento.mouseButton.y)
                    );
                    
                    std::cout << "🖱️ Click en: " << posicionMouse.x << "," << posicionMouse.y << std::endl;
                    
                    // Manejar clicks en botones simples
                    for (auto& boton : botonesSimples) {
                        if (boton->contienePunto(posicionMouse)) {
                            std::cout << "🎯 Botón clickeado!" << std::endl;
                            boton->manejarClick();
                        }
                    }
                }
                break;
                
            case sf::Event::MouseButtonReleased:
                // Reset de estados de botones
                break;
                
            case sf::Event::KeyPressed:
                manejarTeclado(evento);
                break;
                
            default:
                break;
        }
    }
}

void InterfazSFML::manejarTeclado(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    
    std::cout << "⌨️ Tecla presionada: " << evento.key.code << std::endl;
    
    switch (evento.key.code) {
        case sf::Keyboard::Escape:
            if (pantallaActual != Pantalla::MENU_PRINCIPAL) {
                std::cout << "🔙 Volviendo al menú principal" << std::endl;
                cambiarPantalla(Pantalla::MENU_PRINCIPAL);
            } else {
                std::cout << "👋 Cerrando aplicación..." << std::endl;
                ventana.close();
            }
            break;
        case sf::Keyboard::Num1:
            std::cout << "🔢 Tecla 1 - Navegando a Calendarización" << std::endl;
            cambiarPantalla(Pantalla::CALENDARIZACION);
            break;
        case sf::Keyboard::Num2:
            std::cout << "🔢 Tecla 2 - Navegando a Sincronización" << std::endl;
            cambiarPantalla(Pantalla::SINCRONIZACION);
            break;
        default:
            break;
    }
}

void InterfazSFML::actualizar() {
    // Actualización simple
    calcularMetricas();
}

void InterfazSFML::dibujar() {
    ventana.clear(colorFondo);
    
    std::cout << "🎨 Dibujando pantalla: " << static_cast<int>(pantallaActual) << std::endl;
    
    switch (pantallaActual) {
        case Pantalla::MENU_PRINCIPAL:
            dibujarMenuPrincipal();
            break;
        case Pantalla::CALENDARIZACION:
            dibujarPantallaCalendarizacion();
            break;
        case Pantalla::SINCRONIZACION:
            dibujarPantallaSincronizacion();
            break;
        case Pantalla::INFORMACION:
            dibujarPantallaInformacion();
            break;
        default:
            dibujarMenuPrincipal();
            break;
    }
    
    // Dibujar botones simples
    std::cout << "🔘 Dibujando " << botonesSimples.size() << " botones" << std::endl;
    for (auto& boton : botonesSimples) {
        boton->dibujar(ventana);
    }
    
    ventana.display();
}

void InterfazSFML::dibujarMenuPrincipal() {
    limpiarBotones();
    std::cout << "🏠 Dibujando menú principal..." << std::endl;
    
    // Título principal - USANDO TEXTO SIMPLE SIN FUENTE EXTERNA
    dibujarTextoSimple("SIMULADOR DE SISTEMAS OPERATIVOS", sf::Vector2f(200, 50), sf::Color::Blue, 28);
    dibujarTextoSimple("Universidad del Valle de Guatemala", sf::Vector2f(300, 100), sf::Color(70, 130, 180), 20);
    
    // Panel de estado simple
    dibujarRectangulo(sf::Vector2f(50, 150), sf::Vector2f(ANCHO_VENTANA - 100, 100), sf::Color(255, 255, 255, 200), sf::Color::Blue);
    dibujarTextoSimple("ESTADO DEL SISTEMA", sf::Vector2f(70, 170), sf::Color::Blue, 20);
    dibujarTextoSimple("Procesos: " + std::to_string(procesos.size()), sf::Vector2f(70, 200), sf::Color::Black, 16);
    dibujarTextoSimple("Recursos: " + std::to_string(recursos.size()), sf::Vector2f(250, 200), sf::Color::Black, 16);
    dibujarTextoSimple("Acciones: " + std::to_string(acciones.size()), sf::Vector2f(430, 200), sf::Color::Black, 16);
    
    // Botones principales - MÁS GRANDES Y SIMPLES
    float inicioX = 200;
    float inicioY = 300;
    float espacioX = 450;
    float espacioY = 150;
    
    // Crear botones sin usar fuente externa
    crearBotonSimple(sf::Vector2f(inicioX, inicioY), sf::Vector2f(400, 120), 
                    "A. CALENDARIZACION", [this]() {
        std::cout << "🔄 Navegando a Calendarización" << std::endl;
        cambiarPantalla(Pantalla::CALENDARIZACION);
    });
    
    crearBotonSimple(sf::Vector2f(inicioX + espacioX, inicioY), sf::Vector2f(400, 120), 
                    "B. SINCRONIZACION", [this]() {
        std::cout << "🔒 Navegando a Sincronización" << std::endl;
        cambiarPantalla(Pantalla::SINCRONIZACION);
    });
    
    crearBotonSimple(sf::Vector2f(inicioX, inicioY + espacioY), sf::Vector2f(180, 80), 
                    "INFORMACION", [this]() {
        std::cout << "ℹ️ Navegando a Información" << std::endl;
        cambiarPantalla(Pantalla::INFORMACION);
    });
    
    crearBotonSimple(sf::Vector2f(inicioX + 200, inicioY + espacioY), sf::Vector2f(180, 80), 
                    "CONFIGURACION", [this]() {
        std::cout << "⚙️ Navegando a Configuración" << std::endl;
        // Por ahora solo mostrar mensaje
        std::cout << "Configuración no implementada aún" << std::endl;
    });
    
    // Instrucciones
    dibujarTextoSimple("CONTROLES: ESC=Menu, 1=Calendarizacion, 2=Sincronizacion", 
                      sf::Vector2f(50, ALTO_VENTANA - 50), sf::Color::Black, 14);
}

void InterfazSFML::dibujarPantallaCalendarizacion() {
    limpiarBotones();
    std::cout << "📊 Dibujando pantalla de calendarización..." << std::endl;
    
    // Header
    dibujarTextoSimple("A. SIMULACION DE CALENDARIZACION", sf::Vector2f(50, 30), sf::Color::Blue, 24);
    
    // Botón volver
    crearBotonSimple(sf::Vector2f(50, 80), sf::Vector2f(150, 50), "VOLVER", [this]() {
        std::cout << "🔙 Volviendo al menú principal" << std::endl;
        cambiarPantalla(Pantalla::MENU_PRINCIPAL);
    });
    
    // Panel de algoritmos
    dibujarRectangulo(sf::Vector2f(50, 150), sf::Vector2f(ANCHO_VENTANA - 100, 200), sf::Color(250, 250, 250), sf::Color::Blue);
    dibujarTextoSimple("ALGORITMOS DE CALENDARIZACION:", sf::Vector2f(70, 170), sf::Color::Blue, 18);
    
    // Botones de algoritmos
    std::vector<std::string> algoritmos = {"FCFS", "SJF", "Round Robin", "SRTF", "Priority"};
    
    for (size_t i = 0; i < algoritmos.size(); i++) {
        sf::Vector2f btnPos(70 + (i % 3) * 200, 200 + (i / 3) * 80);
        bool seleccionado = (estado.algoritmosSeleccionados.find(algoritmos[i]) != estado.algoritmosSeleccionados.end());
        
        crearBotonSimple(btnPos, sf::Vector2f(180, 60), algoritmos[i], [this, algoritmos, i]() {
            toggleAlgoritmo(algoritmos[i]);
            std::cout << "🔄 Algoritmo " << algoritmos[i] << " toggled" << std::endl;
        }, seleccionado);
    }
    
    // Panel de métricas
    dibujarRectangulo(sf::Vector2f(50, 370), sf::Vector2f(ANCHO_VENTANA - 100, 150), sf::Color(255, 255, 255, 200), sf::Color::Green);
    dibujarTextoSimple("METRICAS DE EFICIENCIA", sf::Vector2f(70, 390), sf::Color::Green, 18);
    dibujarTextoSimple("Tiempo Espera: " + std::to_string((int)metricas.tiempoEsperaPromedio), sf::Vector2f(70, 420), sf::Color::Black, 14);
    dibujarTextoSimple("Tiempo Retorno: " + std::to_string((int)metricas.tiempoRetornoPromedio), sf::Vector2f(300, 420), sf::Color::Black, 14);
    dibujarTextoSimple("Utilizacion CPU: " + std::to_string((int)metricas.utilizacionCPU) + "%", sf::Vector2f(550, 420), sf::Color::Black, 14);
    
    // Controles
    if (!estado.algoritmosSeleccionados.empty()) {
        crearBotonSimple(sf::Vector2f(50, 550), sf::Vector2f(150, 60), "EJECUTAR", [this]() {
            std::cout << "🚀 Iniciando simulación de calendarización" << std::endl;
            iniciarSimulacionCalendarizacion();
        });
    }
    
    crearBotonSimple(sf::Vector2f(220, 550), sf::Vector2f(150, 60), "REINICIAR", [this]() {
        std::cout << "🔄 Reiniciando simulación" << std::endl;
        reiniciarSimulacion();
    });
}

void InterfazSFML::dibujarPantallaSincronizacion() {
    limpiarBotones();
    std::cout << "🔒 Dibujando pantalla de sincronización..." << std::endl;
    
    // Header
    dibujarTextoSimple("B. SIMULACION DE SINCRONIZACION", sf::Vector2f(50, 30), sf::Color::Blue, 24);
    
    // Botón volver
    crearBotonSimple(sf::Vector2f(50, 80), sf::Vector2f(150, 50), "VOLVER", [this]() {
        std::cout << "🔙 Volviendo al menú principal" << std::endl;
        cambiarPantalla(Pantalla::MENU_PRINCIPAL);
    });
    
    // Panel de modos
    dibujarRectangulo(sf::Vector2f(50, 150), sf::Vector2f(ANCHO_VENTANA - 100, 200), sf::Color(250, 250, 250), sf::Color::Blue);
    dibujarTextoSimple("MODOS DE SINCRONIZACION:", sf::Vector2f(70, 170), sf::Color::Blue, 18);
    
    bool mutexSeleccionado = (estado.modoSincronizacion == "Mutex");
    bool semaforoSeleccionado = (estado.modoSincronizacion == "Semaforo");
    
    crearBotonSimple(sf::Vector2f(100, 210), sf::Vector2f(250, 100), "MUTEX", [this]() {
        seleccionarModoSincronizacion("Mutex");
        std::cout << "🔒 Modo Mutex seleccionado" << std::endl;
    }, mutexSeleccionado);
    
    crearBotonSimple(sf::Vector2f(400, 210), sf::Vector2f(250, 100), "SEMAFORO", [this]() {
        seleccionarModoSincronizacion("Semaforo");
        std::cout << "🔢 Modo Semáforo seleccionado" << std::endl;
    }, semaforoSeleccionado);
    
    // Panel de recursos - CORREGIDO: usar color RGB en lugar de Orange
    dibujarRectangulo(sf::Vector2f(50, 370), sf::Vector2f(ANCHO_VENTANA - 100, 150), sf::Color(255, 255, 255, 200), sf::Color(255, 165, 0));
    dibujarTextoSimple("RECURSOS Y ACCIONES", sf::Vector2f(70, 390), sf::Color(255, 165, 0), 18);
    
    for (size_t i = 0; i < recursos.size() && i < 3; i++) {
        std::string info = recursos[i].nombre + ": " + std::to_string(recursos[i].contador);
        dibujarTextoSimple(info, sf::Vector2f(70 + i * 200, 420), sf::Color::Black, 14);
    }
    
    // Controles
    if (!estado.modoSincronizacion.empty()) {
        crearBotonSimple(sf::Vector2f(50, 550), sf::Vector2f(150, 60), "EJECUTAR", [this]() {
            std::cout << "🚀 Iniciando simulación de sincronización: " << estado.modoSincronizacion << std::endl;
            iniciarSimulacionSincronizacion();
        });
    }
}

void InterfazSFML::dibujarPantallaInformacion() {
    limpiarBotones();
    std::cout << "ℹ️ Dibujando pantalla de información..." << std::endl;
    
    // Header
    dibujarTextoSimple("INFORMACION DEL SISTEMA", sf::Vector2f(50, 30), sf::Color::Blue, 24);
    
    // Botón volver
    crearBotonSimple(sf::Vector2f(50, 80), sf::Vector2f(150, 50), "VOLVER", [this]() {
        cambiarPantalla(Pantalla::MENU_PRINCIPAL);
    });
    
    // Información de procesos
    dibujarRectangulo(sf::Vector2f(50, 150), sf::Vector2f(600, 200), sf::Color(255, 255, 255, 200), sf::Color::Blue);
    dibujarTextoSimple("PROCESOS CARGADOS:", sf::Vector2f(70, 170), sf::Color::Blue, 18);
    
    for (size_t i = 0; i < procesos.size() && i < 6; i++) {
        std::string info = procesos[i].pid + " - BT:" + std::to_string(procesos[i].burstTime) + 
                          " AT:" + std::to_string(procesos[i].arrivalTime) + 
                          " P:" + std::to_string(procesos[i].priority);
        dibujarTextoSimple(info, sf::Vector2f(70, 200 + i * 25), sf::Color::Black, 14);
    }
    
    // Información de recursos
    dibujarRectangulo(sf::Vector2f(700, 150), sf::Vector2f(600, 200), sf::Color(255, 255, 255, 200), sf::Color::Green);
    dibujarTextoSimple("RECURSOS DISPONIBLES:", sf::Vector2f(720, 170), sf::Color::Green, 18);
    
    for (size_t i = 0; i < recursos.size() && i < 6; i++) {
        std::string info = recursos[i].nombre + " - Cantidad: " + std::to_string(recursos[i].contador);
        dibujarTextoSimple(info, sf::Vector2f(720, 200 + i * 25), sf::Color::Black, 14);
    }
}

// Métodos auxiliares SIMPLIFICADOS

void InterfazSFML::dibujarTextoSimple(const std::string& texto, sf::Vector2f posicion, sf::Color color, int tamaño) {
    sf::Text textoSFML;
    // NO usar fuente externa, usar la por defecto
    textoSFML.setString(texto);
    textoSFML.setCharacterSize(tamaño);
    textoSFML.setFillColor(color);
    textoSFML.setPosition(posicion);
    textoSFML.setStyle(sf::Text::Bold);
    
    std::cout << "📝 Dibujando texto: '" << texto << "' en (" << posicion.x << "," << posicion.y << ")" << std::endl;
    
    ventana.draw(textoSFML);
}

void InterfazSFML::crearBotonSimple(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& texto, 
                                   std::function<void()> callback, bool seleccionado) {
    std::cout << "🔘 Creando botón simple: " << texto << std::endl;
    
    // Crear botón manualmente sin usar la clase Boton que requiere fuente
    auto boton = std::make_unique<BotonSimple>();
    boton->forma.setPosition(posicion);
    boton->forma.setSize(tamaño);
    boton->forma.setFillColor(seleccionado ? sf::Color::Green : sf::Color(70, 130, 180));
    boton->forma.setOutlineThickness(3);
    boton->forma.setOutlineColor(sf::Color::Black);
    
    boton->texto.setString(texto);
    boton->texto.setCharacterSize(16);
    boton->texto.setFillColor(sf::Color::White);
    boton->texto.setStyle(sf::Text::Bold);
    
    // Centrar texto
    sf::FloatRect textBounds = boton->texto.getLocalBounds();
    boton->texto.setPosition(sf::Vector2f(
        posicion.x + (tamaño.x - textBounds.width) / 2,
        posicion.y + (tamaño.y - textBounds.height) / 2 - 5
    ));
    
    boton->callback = callback;
    boton->posicion = posicion;
    boton->tamaño = tamaño;
    boton->seleccionado = seleccionado;
    
    botonesSimples.push_back(std::move(boton));
}

void InterfazSFML::dibujarRectangulo(sf::Vector2f posicion, sf::Vector2f tamaño, sf::Color color, sf::Color borde) {
    sf::RectangleShape rectangulo;
    rectangulo.setPosition(posicion);
    rectangulo.setSize(tamaño);
    rectangulo.setFillColor(color);
    if (borde != sf::Color::Transparent) {
        rectangulo.setOutlineThickness(2);
        rectangulo.setOutlineColor(borde);
    }
    ventana.draw(rectangulo);
}

// Implementaciones simplificadas de métodos de control

void InterfazSFML::cambiarPantalla(Pantalla nuevaPantalla) {
    pantallaActual = nuevaPantalla;
    limpiarBotones();
    std::cout << "🔄 Cambiando a pantalla: " << static_cast<int>(nuevaPantalla) << std::endl;
}

void InterfazSFML::limpiarBotones() {
    botones.clear();
    botonesSimples.clear();
}

void InterfazSFML::toggleAlgoritmo(const std::string& algoritmo) {
    if (estado.algoritmosSeleccionados.find(algoritmo) != estado.algoritmosSeleccionados.end()) {
        estado.algoritmosSeleccionados.erase(algoritmo);
        std::cout << "❌ Algoritmo " << algoritmo << " deseleccionado" << std::endl;
    } else {
        estado.algoritmosSeleccionados.insert(algoritmo);
        std::cout << "✅ Algoritmo " << algoritmo << " seleccionado" << std::endl;
    }
}

void InterfazSFML::seleccionarModoSincronizacion(const std::string& modo) {
    estado.modoSincronizacion = modo;
    std::cout << "🔧 Modo de sincronización seleccionado: " << modo << std::endl;
}

void InterfazSFML::iniciarSimulacionCalendarizacion() {
    if (estado.algoritmosSeleccionados.empty()) {
        std::cout << "⚠️ Seleccione al menos un algoritmo" << std::endl;
        return;
    }
    
    estado.enEjecucion = true;
    estado.pausada = false;
    estado.cicloActual = 0;
    
    std::cout << "🚀 Simulación de calendarización iniciada con " << estado.algoritmosSeleccionados.size() << " algoritmos" << std::endl;
    
    // Simular ejecución básica
    for (const auto& algoritmo : estado.algoritmosSeleccionados) {
        std::cout << "⚙️ Ejecutando algoritmo: " << algoritmo << std::endl;
        // Aquí iría la lógica de simulación real
    }
}

void InterfazSFML::iniciarSimulacionSincronizacion() {
    if (estado.modoSincronizacion.empty()) {
        std::cout << "⚠️ Seleccione un modo de sincronización" << std::endl;
        return;
    }
    
    estado.enEjecucion = true;
    estado.pausada = false;
    estado.cicloActual = 0;
    
    std::cout << "🔒 Simulación de sincronización iniciada: " << estado.modoSincronizacion << std::endl;
}

void InterfazSFML::reiniciarSimulacion() {
    estado.enEjecucion = false;
    estado.pausada = false;
    estado.cicloActual = 0;
    estado.eventosGantt.clear();
    std::cout << "🔄 Simulación reiniciada" << std::endl;
}

void InterfazSFML::asignarColoresProcesos() {
    std::vector<sf::Color> colores = {
        sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow,
        sf::Color::Magenta, sf::Color::Cyan
    };
    
    for (size_t i = 0; i < procesos.size(); i++) {
        estado.coloresProcesos[procesos[i].pid] = colores[i % colores.size()];
    }
}

void InterfazSFML::calcularMetricas() {
    // Métricas básicas de ejemplo
    metricas.procesosTotales = procesos.size();
    metricas.procesosCompletados = estado.enEjecucion ? estado.cicloActual / 5 : 0;
    metricas.tiempoEsperaPromedio = 5.5;
    metricas.tiempoRetornoPromedio = 12.3;
    metricas.utilizacionCPU = 85.0;
}

// Métodos stub para compilación
void InterfazSFML::manejarClickTimeline(sf::Vector2f posicion) {}
void InterfazSFML::manejarScrollTimeline(float delta) {}
void InterfazSFML::cargarArchivoProcesos() {}
void InterfazSFML::cargarArchivoRecursos() {}
void InterfazSFML::cargarArchivoAcciones() {}
void InterfazSFML::guardarResultados() {}
void InterfazSFML::limpiarDatos() { 
    procesos.clear();
    recursos.clear();
    acciones.clear();
    cargarDatosIniciales();
}
void InterfazSFML::pausarSimulacion() {}
void InterfazSFML::detenerSimulacion() {}
void InterfazSFML::ejecutarAlgoritmo(const std::string& algoritmo) {}
void InterfazSFML::cargarEventosGantt(const std::string& algoritmo) {}
sf::Color InterfazSFML::obtenerColorProceso(const std::string& pid) { return sf::Color::Red; }
void InterfazSFML::configurarQuantum(int nuevoQuantum) {}
void InterfazSFML::mostrarDialogoCarga(const std::string& tipo) {}
void InterfazSFML::activarEntradaTexto(sf::Vector2f posicion, sf::Vector2f tamaño) {}
void InterfazSFML::desactivarEntradaTexto() {}
void InterfazSFML::animarTransicion() {}
sf::Color InterfazSFML::interpolarColor(sf::Color inicio, sf::Color fin, float t) { return inicio; }
void InterfazSFML::mostrarMensaje(const std::string& mensaje, sf::Color color) {}
void InterfazSFML::manejarEntradaTexto(sf::Event& evento) {}
void InterfazSFML::actualizarAnimaciones() {}

// Métodos de dibujo no implementados
void InterfazSFML::dibujarPanelEstado() {}
void InterfazSFML::dibujarPanelAlgoritmos() {}
void InterfazSFML::dibujarPanelConfiguracion() {}
void InterfazSFML::dibujarPanelMetricas() {}
void InterfazSFML::dibujarPanelRecursosAcciones() {}
void InterfazSFML::dibujarControlesSimulacion() {}
void InterfazSFML::dibujarControlesCalendarizacion() {}
void InterfazSFML::dibujarTimelineGantt() {}
void InterfazSFML::dibujarTimelineSincronizacion() {}
void InterfazSFML::dibujarEscalaTiempo(sf::Vector2f pos, float ancho) {}
void InterfazSFML::dibujarLineaTiempoProceso(const Proceso& proceso, sf::Vector2f pos, float ancho, float altura) {}
void InterfazSFML::dibujarAccionEnTimeline(const Accion& accion, sf::Vector2f pos, sf::Vector2f tam) {}
void InterfazSFML::dibujarTablaProcesos() {}
void InterfazSFML::dibujarTablaRecursos() {}
void InterfazSFML::dibujarTablaAcciones() {}
void InterfazSFML::dibujarPantallaConfiguracion() {}
void InterfazSFML::dibujarPantallaCargaArchivos() {}
void InterfazSFML::dibujarPantallaResultados() {}
void InterfazSFML::crearBoton(sf::Vector2f posicion, sf::Vector2f tamaño, const std::string& texto, std::function<void()> callback) {}
void InterfazSFML::dibujarEncabezado(const std::string& titulo, sf::Vector2f posicion) {}
void InterfazSFML::dibujarTexto(const std::string& texto, sf::Vector2f posicion, sf::Color color, int tamaño) {}
void InterfazSFML::dibujarBarraProgreso(sf::Vector2f posicion, sf::Vector2f tamaño, float progreso, sf::Color color) {}
