set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.100.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 43

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:19' 0, '2013-04-04 17:43:02' 43, '2013-04-04 17:43:45' 86, '2013-04-04 17:44:28' 129, '2013-04-04 17:45:11' 172, '2013-04-04 17:45:54' 215, '2013-04-04 17:46:37' 258, '2013-04-04 17:47:20' 301, '2013-04-04 17:48:03' 344, '2013-04-04 17:48:46' 387, '2013-04-04 17:49:29' 430)

f1(x) = (0.000000 < x && x < 301.193886) ? 0.541130*x + 3.334895 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 17:49:35 2013\n192.168.0.100\nunknown" textcolor lt 1

plot 'log/192.168.0.100.log' title '', f1(x) title 'f1(x) = 0.541130*x + 3.334895'
