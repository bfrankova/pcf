set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.38.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 133

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:21' 0, '2013-04-04 17:44:34' 133, '2013-04-04 17:46:47' 266, '2013-04-04 17:49:00' 399, '2013-04-04 17:51:13' 532, '2013-04-04 17:53:26' 665, '2013-04-04 17:55:39' 798, '2013-04-04 17:57:52' 931, '2013-04-04 18:00:05' 1064, '2013-04-04 18:02:18' 1197, '2013-04-04 18:04:31' 1330)

f1(x) = (0.000000 < x && x < 1201.622399) ? 0.498984*x + 11.103660 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:04:36 2013\n192.168.0.38\nunknown" textcolor lt 1

plot 'log/192.168.0.38.log' title '', f1(x) title 'f1(x) = 0.498984*x + 11.103660'
