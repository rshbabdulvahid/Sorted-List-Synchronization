#! /usr/bin/gnuplot

# general plot parameters
set terminal png
set datafile separator ","

set xtics

set title "Operations/second for various threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:32]
set ylabel "Ops/sec"
set logscale y
set output 'lab2b_1.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'list w/mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'list w/spin-lock' with linespoints lc rgb 'green'


set title "Wait-for-lock and Time/Operation for various mutexed threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:32]
set ylabel "ns"
set logscale y
set output 'lab2b_2.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($7) \
     title 'time/op' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($8) \
     title 'wait-for-lock time' with linespoints lc rgb 'green'

set title "Unprotected and Protected Threads running without Failure"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 2
set output 'lab2b_3.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep list-id-none lab2b_list.csv" using ($2):($3) \
     title 'unprotected' with points lc rgb 'green', \
     "< grep list-id-m lab2b_list.csv" using ($2):($3) \
     title 'mutex' with points lc rgb 'red', \
     "< grep list-id-s lab2b_list.csv" using ($2):($3) \
     title 'spin-lock' with points pointtype 1 lc rgb 'blue'     

set title "Operations/second for multi-list: mutex"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:16]
set ylabel "Ops/sec"
set logscale y
set output 'lab2b_4.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=1' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=4' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=8' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=16' with linespoints lc rgb 'purple'

set title "Operations/second for multi-list: spin-lock"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:16]
set ylabel "Ops/sec"
set logscale y
set output 'lab2b_5.png'
set key left top
plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=1' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=4' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=8' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'lists=16' with linespoints lc rgb 'purple'
