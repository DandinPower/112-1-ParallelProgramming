#!/bin/sh
which python3

for i in `seq 1 10`; do
    python3 /home/.grade/HW4/matrix_gen.py 300 500 $i > "/home/.grade/HW4/data-set/data1_${i}" 2> "/home/.grade/HW4/data-set/ans1_${i}"
done

for i in `seq 1 10`; do
    python3 /home/.grade/HW4/matrix_gen.py 1000 2000 $i > "/home/.grade/HW4/data-set/data2_${i}" 2> "/home/.grade/HW4/data-set/ans2_${i}"
done
