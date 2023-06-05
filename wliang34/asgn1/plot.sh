#!/bin/bash
iter=${1:-800}
./monte_carlo -n $iter > f1.dat
echo "0 0 1" > circle.dat
awk '{if (( $5==0  )) print $0}' f1.dat > f1.dat0
awk '{if (( $5==1  )) print $0}' f1.dat > f1.dat1
gnuplot <<END
set terminal pdf
set output "Figure1.pdf" 
set size square
plot [0:1][0:1] "f1.dat0" using 3:4 pt 7 ps 0.3 lc "red" notitle, "f1.dat1" using 3:4 pt 7 ps 0.3 lc "blue" notitle, "circle.dat" using 1:2:3 with circle lc "black" notitle
END
rm f1.dat* circle.dat


echo $(seq 65536) | tr ' ' '\n' > iter.dat
pi=$(echo "scale=5; 4*a(1)" | bc -l)
for i in $(seq 4)
    do
    ./monte_carlo -r $i -n 65536 | awk -v PI=$pi '{if (($2 != "Pi")) print $2-PI}' > temp.dat${i}
    done
paste iter.dat temp.dat* > err.dat

gnuplot <<END
set terminal pdf
set output "Figure2.pdf" 
set title "Monte Carlo Error Estimation" 
set grid xtics
set grid ytics
set ylabel "Error"
set xrange [1:65536]
set yrange [-1:1]
set ytics (-1,-0.5,0,0.5,1) 
set logscale x 4
set xtics (1,4,16,64,256,1024,4096,16384)
plot "err.dat" using 1:2 with lines notitle, "" using 1:3 with lines notitle, "" using 1:4 with lines notitle, "" using 1:5 with lines notitle
END

rm iter.dat temp.dat* err.dat



               
