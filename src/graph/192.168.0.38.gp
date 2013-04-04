set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.38.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 150

set datafile separator '\t'

set x2tics ('2013-04-04 18:14:27' 0, '2013-04-04 18:16:57' 150, '2013-04-04 18:19:27' 300, '2013-04-04 18:21:57' 450, '2013-04-04 18:24:27' 600, '2013-04-04 18:26:57' 750, '2013-04-04 18:29:27' 900, '2013-04-04 18:31:57' 1050, '2013-04-04 18:34:27' 1200, '2013-04-04 18:36:57' 1350, '2013-04-04 18:39:27' 1500)

f1(x) = (0.000000 < x && x < 1501.725695) ? 0.500186*x + 2.882966 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:39:29 2013\n192.168.0.38\nunknown" textcolor lt 1

plot 'log/192.168.0.38.log' title '', f1(x) title 'f1(x) = 0.500186*x + 2.882966'
