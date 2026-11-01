set encoding utf8
set terminal pngcairo enhanced size 1100,650 font "Helvetica,14"

# Estilo general
set border linewidth 1.2 linecolor rgb "#444444"
set style line 100 lc rgb "#bbbbbb" lw 1
set style line 101 lc rgb "#e6e6e6" lw 0.5
set grid xtics ytics mytics ls 100, ls 101
set key outside right center box lc rgb "#bbbbbb" spacing 1.4 samplen 3
set xlabel "Tamaño del problema (n)" offset 0,-0.5
set ylabel "Tiempo promedio de búsqueda (s)" offset -1,0
set format x "%.0s%c"
set xtics 1000000
set xrange [500000:10500000]

set style line 1 lc rgb "#d62728" lw 2.5 pt 7  ps 1.3
set style line 2 lc rgb "#1f77b4" lw 2.5 pt 5  ps 1.3
set style line 3 lc rgb "#2ca02c" lw 2.5 pt 9  ps 1.4
set style line 4 lc rgb "#ff7f0e" lw 2.5 pt 13 ps 1.4
set style line 5 lc rgb "#9467bd" lw 2.5 pt 11 ps 1.4

# Gráfica 1: los cinco algoritmos (escala logarítmica en y)
set output "comparativa.png"
set title "Comparativa del tiempo promedio de búsqueda (tiempo real)" font "Helvetica-Bold,16"
set logscale y
set format y "10^{%L}"

plot "tiempos.dat" using 1:2 with linespoints ls 1 title "Secuencial", \
     ""            using 1:3 with linespoints ls 2 title "ABB", \
     ""            using 1:4 with linespoints ls 3 title "Binaria", \
     ""            using 1:5 with linespoints ls 4 title "Exponencial", \
     ""            using 1:6 with linespoints ls 5 title "Fibonacci"

# Gráfica 2: los cuatro algoritmos logarítmicos (escala lineal en y)
set output "comparativa_log.png"
set title "Tiempo promedio de búsqueda: ABB, Binaria, Exponencial y Fibonacci" font "Helvetica-Bold,16"
unset logscale y
set yrange [0:3e-5]
set format y "%.1t×10^{%T}"
set mytics 5

plot "tiempos.dat" using 1:3 with linespoints ls 2 title "ABB", \
     ""            using 1:4 with linespoints ls 3 title "Binaria", \
     ""            using 1:5 with linespoints ls 4 title "Exponencial", \
     ""            using 1:6 with linespoints ls 5 title "Fibonacci"

unset output