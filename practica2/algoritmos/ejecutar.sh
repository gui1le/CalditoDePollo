#!/bin/bash
# ==============================================================================
# ejecutar.sh
# V 1.0 octubre 2026
#
# Alumnos que lo implementan:
#     Duran de la Rosa Ailed Citlalli
#     Espíritu Martínez Guillermo
#     Hernández Ramírez Andrés
#     Vicente Retana Laura Itzel
#
# Compila un algoritmo de búsqueda y lo ejecuta para diez tamaños de n:
# de 1,000,000 a 10,000,000, en incrementos de 1,000,000.
#
# El programa de C debe:
#   - Recibir n como único argumento.
#   - Leer los números desde la entrada estándar.
#   - Realizar las 20 búsquedas.
#   - Imprimir sus promedios después del encabezado "RESULTADOS".
#
# Uso, desde practica2/algoritmos:
# bash ejecutar.sh secuencial /ruta/al/archivo/numeros10millones.txt
# ==============================================================================

# Detener el script ante errores, incluso dentro de una tubería.
set -euo pipefail

# 1. VALIDAR LOS ARGUMENTOS
if [ "$#" -ne 2 ]; then
    echo "Uso: bash ejecutar.sh <carpeta_algoritmo> <archivo_numeros>"
    echo "Ejemplo: bash ejecutar.sh secuencial ./numeros10millones.txt"
    exit 1
fi

CARPETA="$1"
ARCHIVO_NUMEROS="$2"

# Comprobar la entrada antes de cambiar de directorio.
if [ ! -f "$ARCHIVO_NUMEROS" ] || [ ! -r "$ARCHIVO_NUMEROS" ]; then
    echo "Error: no se puede leer '$ARCHIVO_NUMEROS'."
    exit 1
fi

# Convertir la ruta de los números en absoluta.
ARCHIVO_NUMEROS="$(cd -- "$(dirname -- "$ARCHIVO_NUMEROS")" &&
    pwd)/$(basename -- "$ARCHIVO_NUMEROS")"

# Trabajar desde la ubicación del script.
DIRECTORIO_SCRIPT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$DIRECTORIO_SCRIPT"

# 2. VALIDAR LA CARPETA DEL ALGORITMO
# Se espera una carpeta directamente dentro de algoritmos.
if [[ "$CARPETA" == */* || "$CARPETA" == "." || "$CARPETA" == ".." ]]; then
    echo "Error: indica solo el nombre de la carpeta, por ejemplo: secuencial."
    exit 1
fi

if [ ! -d "$CARPETA" ]; then
    echo "Error: no existe la carpeta '$CARPETA' junto al script."
    exit 1
fi

cd -- "$CARPETA"

# 3. LOCALIZAR EL CÓDIGO FUENTE
shopt -s nullglob
ARCHIVOS_C=(*.c)

if [ "${#ARCHIVOS_C[@]}" -ne 1 ]; then
    echo "Error: '$CARPETA' debe contener exactamente un archivo .c."
    exit 1
fi

ARCHIVO_C="${ARCHIVOS_C[0]}"

# 4. COMPROBAR LA BIBLIOTECA Y EL COMPILADOR
if [ ! -f "../../tiempos/tiempo.c" ] ||
   [ ! -f "../../tiempos/tiempo.h" ]; then
    echo "Error: faltan tiempo.c o tiempoHeader.h en practica2/tiempos."
    exit 1
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "Error: GCC no está disponible en esta terminal."
    exit 1
fi

# 5. COMPILAR UNA SOLA VEZ
echo "Compilando $ARCHIVO_C..."

if ! gcc -std=gnu11 -Wall -Wextra \
    "$ARCHIVO_C" ../../tiempos/tiempo.c -o programa; then
    echo "Error de compilación. No se ejecutaron las pruebas."
    exit 1
fi

# 6. TAMAÑOS SOLICITADOS EN LA PRÁCTICA
TAMANOS=(
    1000000
    2000000
    3000000
    4000000
    5000000
    6000000
    7000000
    8000000
    9000000
    10000000
)

# 7. PREPARAR LOS ARCHIVOS DE RESULTADOS
# Cada ejecución del script obtiene una carpeta propia.
mkdir -p ../../analisis/tiempos

DIRECTORIO_SALIDA="$(mktemp -d \
    "../../analisis/tiempos/${CARPETA}_XXXXXXXX")"

DETALLE="$DIRECTORIO_SALIDA/detalle.txt"
PROMEDIOS="$DIRECTORIO_SALIDA/promedios.txt"

echo "=== DETALLE DE BUSQUEDAS: $CARPETA ===" > "$DETALLE"
echo "=== TIEMPOS PROMEDIO: $CARPETA ===" > "$PROMEDIOS"

# 8. EJECUTAR TODOS LOS TAMAÑOS
for n in "${TAMANOS[@]}"; do
    echo "Ejecutando $CARPETA para n = $n..."

    # El programa lee los primeros n números del archivo.
    # tee conserva la salida completa.
    # sed extrae desde RESULTADOS hasta el final.
    if ! ./programa "$n" < "$ARCHIVO_NUMEROS" |
        tee -a "$DETALLE" |
        sed -n '/================ RESULTADOS/,$p' >> "$PROMEDIOS"; then

        echo "Error durante la prueba con n = $n."
        echo "La ejecución quedó incompleta. Revisa: $DIRECTORIO_SALIDA"
        exit 1
    fi

    echo "" >> "$DETALLE"
    echo "" >> "$PROMEDIOS"
done

echo "--------------------------------------------------"
echo "¡Listo! Se completaron los diez tamaños."
echo "Resultados guardados en:"
echo "$(cd -- "$DIRECTORIO_SALIDA" && pwd)"
echo "  detalle.txt   : resultados de cada búsqueda."
echo "  promedios.txt : tiempos promedio para cada n."