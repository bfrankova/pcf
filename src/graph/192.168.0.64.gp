set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.64.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 85

set datafile separator '\t'

set x2tics ('2013-04-04 18:15:02' 0, '2013-04-04 18:16:27' 85, '2013-04-04 18:17:52' 170, '2013-04-04 18:19:17' 255, '2013-04-04 18:20:42' 340, '2013-04-04 18:22:07' 425, '2013-04-04 18:23:32' 510, '2013-04-04 18:24:57' 595, '2013-04-04 18:26:22' 680, '2013-04-04 18:27:47' 765, '2013-04-04 18:29:12' 850)

f1(x) = (0.000000 < x && x < 602.434885) ? 0.521318*x + 4.536401 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:29:18 2013\n192.168.0.64\nunknown" textcolor lt 1

plot 'log/192.168.0.64.log' title '', f1(x) title 'f1(x) = 0.521318*x + 4.536401'
