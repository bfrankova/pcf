set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.100.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 24

set datafile separator '\t'

set x2tics ('2013-04-04 18:21:17' 0, '2013-04-04 18:21:41' 24, '2013-04-04 18:22:05' 48, '2013-04-04 18:22:29' 72, '2013-04-04 18:22:53' 96, '2013-04-04 18:23:17' 120, '2013-04-04 18:23:41' 144, '2013-04-04 18:24:05' 168, '2013-04-04 18:24:29' 192, '2013-04-04 18:24:53' 216, '2013-04-04 18:25:17' 240)



set grid xtics x2tics ytics
set title "Thu Apr  4 18:25:19 2013\n192.168.0.100\nunknown" textcolor lt 1

plot 'log/192.168.0.100.log' title ''
