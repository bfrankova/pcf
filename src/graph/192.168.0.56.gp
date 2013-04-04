set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.56.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 150

set datafile separator '\t'

set x2tics ('2013-04-04 18:14:28' 0, '2013-04-04 18:16:58' 150, '2013-04-04 18:19:28' 300, '2013-04-04 18:21:58' 450, '2013-04-04 18:24:28' 600, '2013-04-04 18:26:58' 750, '2013-04-04 18:29:28' 900, '2013-04-04 18:31:58' 1050, '2013-04-04 18:34:28' 1200, '2013-04-04 18:36:58' 1350, '2013-04-04 18:39:28' 1500)

f1(x) = (0.000000 < x && x < 1504.139768) ? 0.493954*x + 9.574373 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:39:32 2013\n192.168.0.56\nunknown" textcolor lt 1

plot 'log/192.168.0.56.log' title '', f1(x) title 'f1(x) = 0.493954*x + 9.574373'
