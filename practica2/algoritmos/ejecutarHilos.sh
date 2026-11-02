#!/bin/bash
# ==============================================================================
# ejecutarHilos.sh
# V 1.0 noviembre 2026
#
# Alumnos que lo implementan:
#     Duran de la Rosa Ailed Citlalli
#     Espíritu Martínez Guillermo
#     Hernández Ramírez Andrés
#     Vicente Retana Laura Itzel
#
# Descripción:
# Compila las versiones con hilos de los cinco algoritmos de búsqueda:
# secuencial, ABB, binaria, exponencial y Fibonacci.
# Ejecuta cada programa para diez tamaños de n: de 1,000,000 a 10,000,000,
# en incrementos de 1,000,000, con la cantidad de hilos indicada.
#
# Cada programa de C abre y lee directamente su archivo de datos.
# Formatea la salida agrupando los números encontrados y no encontrados,
# conservando las posiciones, los pasos y los tiempos que imprime el programa.
# Conserva también la comparación sin/con hilos cuando el programa la incluye.
# Secuencial mide por búsqueda; las otras versiones trabajan por lotes
# de 20 consultas. Sus medidas se mantienen identificadas en los resultados.
#
# Salida:
# Genera un archivo <algoritmo>_<hilos>h_tiempos.txt por algoritmo dentro de:
# practica2/analisis/tiempos/hilos/ejecucion_<hilos>h_XXXXXX/
# Crea una carpeta distinta en cada ejecución para conservar las anteriores.
# Si una prueba falla, conserva los resultados incompletos en un .parcial.
#
# Requiere:
# Bash, GCC con soporte pthread, awk y mktemp.
# Los archivos <algoritmo>Hilos.c dentro de sus respectivas carpetas.
# tiempo.c y tiempo.h dentro de practica2/analisis/.
# 10millones.txt y numeros10millones.txt dentro de practica2/algoritmos/.
#
# Uso desde practica2/algoritmos:
# bash ejecutarHilos.sh 4
# Si se omite la cantidad de hilos, utiliza 4. Se permiten de 1 a 20.
# ==============================================================================

# Detener el script ante errores y variables no definidas.
set -euo pipefail
export LC_ALL=C

if [ "$#" -gt 1 ]; then
    echo "Uso: bash ejecutarHilos.sh [numero_hilos]" >&2
    exit 1
fi
HILOS="${1:-4}"
# El limite comun es 20: los algoritmos por lote reparten 20 consultas.
if [[ ! "$HILOS" =~ ^([1-9]|1[0-9]|20)$ ]]; then
    echo "Error: numero_hilos debe ser un entero entre 1 y 20." >&2
    exit 1
fi

DIRECTORIO_SCRIPT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$DIRECTORIO_SCRIPT"
ALGORITMOS=(secuencial abb binaria exponencial fibonacci)
TAMANOS=(
    1000000 2000000 3000000 4000000 5000000
    6000000 7000000 8000000 9000000 10000000
)

for herramienta in gcc awk mktemp; do
    if ! command -v "$herramienta" >/dev/null 2>&1; then
        echo "Error: falta el comando $herramienta." >&2
        exit 1
    fi
done

# Validar todos los archivos antes de empezar a compilar o medir.
for archivo in ../analisis/tiempo.c ../analisis/tiempo.h \
               10millones.txt numeros10millones.txt; do
    if [ ! -r "$archivo" ]; then
        echo "Error: falta el archivo o no se puede leer: $DIRECTORIO_SCRIPT/$archivo" >&2
        exit 1
    fi
done
for algoritmo in "${ALGORITMOS[@]}"; do
    fuente="$algoritmo/${algoritmo}Hilos.c"
    if [ ! -r "$fuente" ]; then
        echo "Error: no se puede leer $DIRECTORIO_SCRIPT/$fuente" >&2
        exit 1
    fi
done

# Una carpeta nueva por ejecucion conserva las mediciones anteriores.
BASE_RESULTADOS="$DIRECTORIO_SCRIPT/../analisis/tiempos/hilos"
mkdir -p -- "$BASE_RESULTADOS"
BASE_RESULTADOS="$(cd -- "$BASE_RESULTADOS" && pwd)"
DIR_RESULTADOS="$(mktemp -d "$BASE_RESULTADOS/ejecucion_${HILOS}h_XXXXXX")"
TEMPORAL="$(mktemp -d)"
trap 'rm -rf -- "$TEMPORAL"' EXIT
trap 'echo "Pruebas interrumpidas. Resultados parciales en: $DIR_RESULTADOS" >&2; exit 130' INT
trap 'echo "Pruebas interrumpidas. Resultados parciales en: $DIR_RESULTADOS" >&2; exit 143' TERM

echo "Hilos: $HILOS | Algoritmos: 5 | Tamanos: 10"
echo "Resultados: $DIR_RESULTADOS"

# Compilar exactamente el fuente con hilos, sin incluir el original con otro main.
# Mantener las opciones de optimizacion usadas en los comandos anteriores.
for algoritmo in "${ALGORITMOS[@]}"; do
    echo "Compilando ${algoritmo}Hilos.c..."
    if ! gcc -std=gnu11 -Wall -Wextra -pthread \
        "$algoritmo/${algoritmo}Hilos.c" ../analisis/tiempo.c \
        -o "$TEMPORAL/${algoritmo}Hilos"; then
        echo "Error al compilar $algoritmo. No se ejecutaron las pruebas." >&2
        exit 1
    fi
done

# Agrupar encontrados/no encontrados sin confundir indices con pasos.
# Se conserva el texto completo de cada resultado y todos los tiempos.
formatear_salida() {
    awk '
    /^Numero / {
        if ($0 ~ /NO encontrado/) no[++cantidad_no] = $0
        else si[++cantidad_si] = $0
        next
    }
    /^================ RESULTADOS/ {
        print "Numeros NO encontrados:"
        if (!cantidad_no) print "Ninguno."
        for (i = 1; i <= cantidad_no; i++) print no[i]
        print ""
        print "Numeros SI encontrados:"
        if (!cantidad_si) print "Ninguno."
        for (i = 1; i <= cantidad_si; i++) print si[i]
        print ""
    }
    { print }
    ' "$1"
}

# Ejecutar un programa a la vez evita que compitan entre si durante las mediciones.
for algoritmo in "${ALGORITMOS[@]}"; do
    FINAL="$DIR_RESULTADOS/${algoritmo}_${HILOS}h_tiempos.txt"
    PARCIAL="$FINAL.parcial"
    {
        echo "=== RESULTADOS: $algoritmo | HILOS: $HILOS ==="
        echo "Fuente: ${algoritmo}Hilos.c"
        echo "Compilacion: gcc -std=gnu11 -Wall -Wextra -pthread"
        echo "Cada n se ejecuta una vez; los programas por lote usan su repeticion predeterminada (1)."
        if [ "$algoritmo" = secuencial ]; then
            echo "Medida: promedio de 20 busquedas individuales con hilos."
        else
            echo "Medida: lote de 20 consultas y tiempo amortizado por consulta."
            echo "Las versiones y comparaciones mostradas son las incluidas en el fuente."
        fi
    } > "$PARCIAL"

    for n in "${TAMANOS[@]}"; do
        echo "Ejecutando $algoritmo | n = $n | hilos = $HILOS..."
        printf '\n=== PRUEBA n = %s | HILOS = %s ===\n' "$n" "$HILOS" >> "$PARCIAL"
        # El directorio de trabajo permite que ../10millones.txt y
        # ../numeros10millones.txt se resuelvan como en los programas originales.
        if ! (cd -- "$DIRECTORIO_SCRIPT/$algoritmo" &&
              "$TEMPORAL/${algoritmo}Hilos" "$n" "$HILOS") \
              > "$TEMPORAL/salida.txt" 2> "$TEMPORAL/error.txt"; then
            cat "$TEMPORAL/salida.txt" "$TEMPORAL/error.txt" >> "$PARCIAL"
            echo "ERROR: prueba incompleta para n = $n." >> "$PARCIAL"
            cat "$TEMPORAL/error.txt" >&2
            echo "Se detuvieron las pruebas. Revisa: $PARCIAL" >&2
            exit 1
        fi
        formatear_salida "$TEMPORAL/salida.txt" >> "$PARCIAL"
        if [ -s "$TEMPORAL/error.txt" ]; then
            cat "$TEMPORAL/error.txt" >> "$PARCIAL"
        fi
    done
    echo "=== COMPLETO: los diez tamanos terminaron correctamente ===" >> "$PARCIAL"
    mv -- "$PARCIAL" "$FINAL"
    echo "Guardado: $FINAL"
done

echo "Listo: terminaron las 50 ejecuciones (5 algoritmos x 10 tamanos)."
echo "Resultados: $DIR_RESULTADOS"
