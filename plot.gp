reset

set grid

set term epslatex standalone colour solid 'phv' header '\usepackage{bm}'

set key left at first -450, 0.95
set ylabel "relative reconstructed amplitude"
set xlabel "relative time of the pulse maximum [ns]"
set out "amplitude.tex"
p [][-1.2:+1.2] "biases.dat" u 1:2 i 0 t "EB" w l lt 3, "" u 1:2 i 1 t "EE" w l lt 1
set out
!pdflatex amplitude.tex

set key left at first -57.5, 75
set ylabel "reconstructed time"
set xlabel "relative time of the pulse maximum [ns]"
set out "time.tex"
p [-70:70][-100:100] x not w l lt -1 lc rgb "#aaaaaa", "biases.dat" u 1:3 i 0 t "EB" w l lt 3, "" u 1:3 i 1 t "EE" w l lt 1
set out
!pdflatex time.tex
