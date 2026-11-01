#!/bin/bash
BASE="$(cd "$(dirname "$0")" && pwd)"
DIR="$BASE/../tiempos"
SALIDA="$BASE/tiempos.dat"

awk '
FNR==1 { col++ }
/N consultadas/ { n = $NF }
/real promedio/ {
    t[n, col] = $(NF-1)
    if (!(n in visto)) { visto[n] = 1; ns[++k] = n }
}
END {
    print "# n secuencial abb binaria exponencial fibonacci"
    for (i = 1; i <= k; i++) {
        n = ns[i]
        printf "%s", n
        for (c = 1; c <= 5; c++) printf " %s", ((n, c) in t) ? t[n, c] : "NaN"
        printf "\n"
    }
}' "$DIR/secuencial_tiempos.txt" \
   "$DIR/abb_tiempos.txt" \
   "$DIR/binaria_tiempos.txt" \
   "$DIR/exponencial_tiempos.txt" \
   "$DIR/fibonacci_tiempos.txt" > "$SALIDA"