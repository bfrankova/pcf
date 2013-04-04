set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.54.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 133

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:19' 0, '2013-04-04 17:44:32' 133, '2013-04-04 17:46:45' 266, '2013-04-04 17:48:58' 399, '2013-04-04 17:51:11' 532, '2013-04-04 17:53:24' 665, '2013-04-04 17:55:37' 798, '2013-04-04 17:57:50' 931, '2013-04-04 18:00:03' 1064, '2013-04-04 18:02:16' 1197, '2013-04-04 18:04:29' 1330)

f1(x) = (0.000000 < x && x < 1206.013253) ? -0.003397*x + 1.119858 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:04:35 2013\n192.168.0.54\n192.168.0.49" textcolor lt 2

plot 'log/192.168.0.54.log' title '', f1(x) title 'f1(x) = -0.003397*x + 1.119858'
