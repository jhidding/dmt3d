#!/bin/bash

gnuplot << EOF
	set term pngcairo background '#000000' size 2048, 2048
	set output 'web.$1.png'

	set cbrange [0.05:100]
	set xrange [0:80]
	set yrange [0:80]
	set log cb

	unset xtics
	unset ytics
	unset colorbox
	unset key

	plot \
		'130616.regt.$1.conan' i 0 w lines lc palette, \
		'' i 1 u 1:2:(sqrt(\$3)) w points lc rgb '#cccc00' pt 7 ps variable
EOF

