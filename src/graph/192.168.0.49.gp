set encoding iso_8859_2
set terminal svg
set output 'www/graph/192.168.0.49.svg'

set x2label 'Date and time'
set xlabel 'Elapsed time [s]'
set ylabel 'Offset [ms]'

set x2tics axis in rotate by 270 textcolor lt 4
set xtics mirror 0, 74

set datafile separator '\t'

set x2tics ('2013-04-04 18:15:22' 0, '2013-04-04 18:16:36' 74, '2013-04-04 18:17:50' 148, '2013-04-04 18:19:04' 222, '2013-04-04 18:20:18' 296, '2013-04-04 18:21:32' 370, '2013-04-04 18:22:46' 444, '2013-04-04 18:24:00' 518, '2013-04-04 18:25:14' 592, '2013-04-04 18:26:28' 666, '2013-04-04 18:27:42' 740)

f1(x) = (0.000000 < x && x < 743.987532) ? 0.000026*x + 2.774184 : 1/0


set grid xtics x2tics ytics
set title "Thu Apr  4 18:27:46 2013\n192.168.0.49\nunknown" textcolor lt 1

plot 'log/192.168.0.49.log' title '', f1(x) title 'f1(x) = 0.000026*x + 2.774184'
