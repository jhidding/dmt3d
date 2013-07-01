#!/bin/bash

gnuplot << EOF
	set term pngcairo background '#000000' size 2048, 2048
	set output 'za.png'

	#set cbrange [0.05:100]
	set xrange [0:80]
	set yrange [0:80]
	#set log cb

	unset xtics
	unset ytics
	unset colorbox
	unset key

	set style fill transparent solid 0.2
	set view map
	splot \
		'za.txt' u (\$1 + $1*\$3):(\$2 + $1*\$4):(0) w pm3d
EOF

