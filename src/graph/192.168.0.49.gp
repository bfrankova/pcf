set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.49.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 32

set datafile separator '\t'

set x2tics ('2013-04-04 17:53:52' 0, '2013-04-04 17:54:24' 32, '2013-04-04 17:54:56' 64, '2013-04-04 17:55:28' 96, '2013-04-04 17:56:00' 128, '2013-04-04 17:56:32' 160, '2013-04-04 17:57:04' 192, '2013-04-04 17:57:36' 224, '2013-04-04 17:58:08' 256, '2013-04-04 17:58:40' 288, '2013-04-04 17:59:12' 320)

f1(x) = (0.000000 < x && x < 325.448914) ? -0.002420*x + 2.742480 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 17:59:18 2013\n192.168.0.49\n192.168.0.54" textcolor lt 2

plot 'log/192.168.0.49.log' title '', f1(x) title 'f1(x) = -0.002420*x + 2.742480'
