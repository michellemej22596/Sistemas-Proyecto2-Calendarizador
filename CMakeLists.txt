cmake_minimum_required(VERSION 3.16)
project(SimuladorSO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detectar arquitectura de Mac automáticamente
if(APPLE)
    # Obtener la arquitectura del sistema
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE SYSTEM_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    message(STATUS "Arquitectura detectada: ${SYSTEM_ARCH}")
    
    if(SYSTEM_ARCH STREQUAL "arm64")
        # Mac con Apple Silicon
        set(CMAKE_OSX_ARCHITECTURES "arm64")
        set(HOMEBREW_PREFIX "/opt/homebrew")
        message(STATUS "Configurando para Apple Silicon (ARM64)")
    else()
        # Mac con Intel
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
        set(HOMEBREW_PREFIX "/usr/local")
        message(STATUS "Configurando para Intel (x86_64)")
    endif()
    
    # Configurar rutas de Homebrew
    include_directories(${HOMEBREW_PREFIX}/include)
    link_directories(${HOMEBREW_PREFIX}/lib)
    
    # Agregar rutas de búsqueda para pkg-config
    set(ENV{PKG_CONFIG_PATH} "${HOMEBREW_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

# Encontrar paquetes necesarios
find_package(OpenGL REQUIRED)

# Buscar GLFW y GLEW manualmente si no se encuentran
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(GLFW3 QUIET glfw3)
    pkg_check_modules(GLEW QUIET glew)
endif()

# Si no se encuentran con pkg-config, buscar manualmente
if(NOT GLFW3_FOUND)
    find_library(GLFW_LIBRARY 
        NAMES glfw glfw3
        PATHS ${HOMEBREW_PREFIX}/lib
        NO_DEFAULT_PATH
    )
    find_path(GLFW_INCLUDE_DIR 
        NAMES GLFW/glfw3.h
        PATHS ${HOMEBREW_PREFIX}/include
        NO_DEFAULT_PATH
    )
    
    if(GLFW_LIBRARY AND GLFW_INCLUDE_DIR)
        set(GLFW3_FOUND TRUE)
        set(GLFW3_LIBRARIES ${GLFW_LIBRARY})
        set(GLFW3_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
        message(STATUS "GLFW encontrado manualmente: ${GLFW_LIBRARY}")
    endif()
endif()

if(NOT GLEW_FOUND)
    find_library(GLEW_LIBRARY 
        NAMES GLEW glew
        PATHS ${HOMEBREW_PREFIX}/lib
        NO_DEFAULT_PATH
    )
    find_path(GLEW_INCLUDE_DIR 
        NAMES GL/glew.h
        PATHS ${HOMEBREW_PREFIX}/include
        NO_DEFAULT_PATH
    )
    
    if(GLEW_LIBRARY AND GLEW_INCLUDE_DIR)
        set(GLEW_FOUND TRUE)
        set(GLEW_LIBRARIES ${GLEW_LIBRARY})
        set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
        message(STATUS "GLEW encontrado manualmente: ${GLEW_LIBRARY}")
    endif()
endif()

# Verificar que se encontraron las dependencias
if(NOT GLFW3_FOUND)
    message(FATAL_ERROR "GLFW no encontrado. Instala con: brew install glfw")
endif()

if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW no encontrado. Instala con: brew install glew")
endif()

# Dear ImGui sources
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Verificar que los archivos existen
set(REQUIRED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simulador.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sincronizacion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recurso.cpp
)

foreach(SOURCE_FILE ${REQUIRED_SOURCES})
    if(NOT EXISTS ${SOURCE_FILE})
        message(FATAL_ERROR "No se encuentra el archivo: ${SOURCE_FILE}")
    endif()
endforeach()

# Tu código existente
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simulador.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sincronizacion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recurso.cpp
    ${IMGUI_SOURCES}
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Incluir directorios
target_include_directories(${PROJECT_NAME} PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Enlazar librerías
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    ${GLFW3_LIBRARIES}
    ${GLEW_LIBRARIES}
)

# Para Mac, agregar frameworks necesarios
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# Copiar archivos de datos al directorio de build
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Información de debug
message(STATUS "=== Información de compilación ===")
message(STATUS "Arquitectura: ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "Homebrew prefix: ${HOMEBREW_PREFIX}")
message(STATUS "GLFW: ${GLFW3_LIBRARIES}")
message(STATUS "GLEW: ${GLEW_LIBRARIES}")
message(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
