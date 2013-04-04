set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.54.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 120

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:19' 0, '2013-04-04 17:44:19' 120, '2013-04-04 17:46:19' 240, '2013-04-04 17:48:19' 360, '2013-04-04 17:50:19' 480, '2013-04-04 17:52:19' 600, '2013-04-04 17:54:19' 720, '2013-04-04 17:56:19' 840, '2013-04-04 17:58:19' 960, '2013-04-04 18:00:19' 1080, '2013-04-04 18:02:19' 1200)

f1(x) = (0.000000 < x && x < 1206.013253) ? -0.003339*x + 1.067798 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:02:25 2013\n192.168.0.54\n192.168.0.49" textcolor lt 2

plot 'log/192.168.0.54.log' title '', f1(x) title 'f1(x) = -0.003339*x + 1.067798'
