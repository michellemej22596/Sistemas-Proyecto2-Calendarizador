#!/bin/bash

echo "=== Solucionador de Problemas de Arquitectura Mac ==="

# Detectar arquitectura
ARCH=$(uname -m)
echo "Arquitectura detectada: $ARCH"

if [ "$ARCH" = "arm64" ]; then
    echo "🍎 Mac con Apple Silicon (M1/M2/M3)"
    HOMEBREW_PREFIX="/opt/homebrew"
else
    echo "💻 Mac con Intel"
    HOMEBREW_PREFIX="/usr/local"
fi

echo "Homebrew prefix: $HOMEBREW_PREFIX"

# Verificar que Homebrew esté instalado en la ubicación correcta
if [ ! -d "$HOMEBREW_PREFIX" ]; then
    echo "❌ Homebrew no encontrado en $HOMEBREW_PREFIX"
    echo "Reinstalando Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Configurar PATH para esta sesión
export PATH="$HOMEBREW_PREFIX/bin:$PATH"

# Verificar e instalar dependencias
echo "🔍 Verificando dependencias..."

if ! command -v pkg-config &> /dev/null; then
    echo "Instalando pkg-config..."
    brew install pkg-config
fi

if ! brew list glfw &>/dev/null; then
    echo "Instalando GLFW..."
    brew install glfw
else
    echo "✅ GLFW ya está instalado"
fi

if ! brew list glew &>/dev/null; then
    echo "Instalando GLEW..."
    brew install glew
else
    echo "✅ GLEW ya está instalado"
fi

if ! brew list cmake &>/dev/null; then
    echo "Instalando CMake..."
    brew install cmake
else
    echo "✅ CMake ya está instalado"
fi

# Verificar que las librerías estén en la arquitectura correcta
echo "🔍 Verificando arquitecturas de librerías..."

GLFW_LIB=$(find $HOMEBREW_PREFIX/lib -name "libglfw*" | head -1)
GLEW_LIB=$(find $HOMEBREW_PREFIX/lib -name "libGLEW*" | head -1)

if [ -n "$GLFW_LIB" ]; then
    echo "GLFW encontrado: $GLFW_LIB"
    file "$GLFW_LIB" | grep -q "$ARCH" && echo "✅ GLFW arquitectura correcta" || echo "❌ GLFW arquitectura incorrecta"
fi

if [ -n "$GLEW_LIB" ]; then
    echo "GLEW encontrado: $GLEW_LIB"
    file "$GLEW_LIB" | grep -q "$ARCH" && echo "✅ GLEW arquitectura correcta" || echo "❌ GLEW arquitectura incorrecta"
fi

# Limpiar build anterior
echo "🧹 Limpiando build anterior..."
rm -rf build/*

# Crear directorio build si no existe
mkdir -p build

echo "🚀 Compilando con arquitectura correcta..."
cd build

# Configurar CMake con la arquitectura correcta
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
    -DCMAKE_PREFIX_PATH="$HOMEBREW_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release

if [ $? -eq 0 ]; then
    echo "✅ CMake configurado correctamente"
    
    # Compilar
    make -j$(sysctl -n hw.ncpu)
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "🎉 ¡Compilación exitosa!"
        echo ""
        echo "Para ejecutar:"
        echo "  cd build"
        echo "  ./SimuladorSO"
        echo ""
        echo "O directamente:"
        echo "  ./build/SimuladorSO"
    else
        echo "❌ Error en la compilación"
        echo "Revisa los errores arriba"
    fi
else
    echo "❌ Error en la configuración de CMake"
    echo "Información de debug:"
    echo "Homebrew prefix: $HOMEBREW_PREFIX"
    echo "Arquitectura: $ARCH"
    echo "PATH: $PATH"
fi
