set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.64.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 83

set datafile separator '\t'

set x2tics ('2013-04-04 17:42:23' 0, '2013-04-04 17:43:46' 83, '2013-04-04 17:45:09' 166, '2013-04-04 17:46:32' 249, '2013-04-04 17:47:55' 332, '2013-04-04 17:49:18' 415, '2013-04-04 17:50:41' 498, '2013-04-04 17:52:04' 581, '2013-04-04 17:53:27' 664, '2013-04-04 17:54:50' 747, '2013-04-04 17:56:13' 830)

f1(x) = (0.000000 < x && x < 604.420242) ? 0.520050*x + 2.813493 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 17:56:19 2013\n192.168.0.64\nunknown" textcolor lt 1

plot 'log/192.168.0.64.log' title '', f1(x) title 'f1(x) = 0.520050*x + 2.813493'
