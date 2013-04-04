set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.38.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 120

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:21' 0, '2013-04-04 17:44:21' 120, '2013-04-04 17:46:21' 240, '2013-04-04 17:48:21' 360, '2013-04-04 17:50:21' 480, '2013-04-04 17:52:21' 600, '2013-04-04 17:54:21' 720, '2013-04-04 17:56:21' 840, '2013-04-04 17:58:21' 960, '2013-04-04 18:00:21' 1080, '2013-04-04 18:02:21' 1200)

f1(x) = (0.000000 < x && x < 1201.622399) ? 0.501457*x + 8.543965 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:02:22 2013\n192.168.0.38\nunknown" textcolor lt 1

plot 'log/192.168.0.38.log' title '', f1(x) title 'f1(x) = 0.501457*x + 8.543965'
