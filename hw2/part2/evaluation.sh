#!/bin/bash

run_mandelbrot() {
    local version=$1
    local view=$2
    local threads=("1" "2" "3" "4" "8")

    for thread in "${threads[@]}"; do
        ./mandelbrot -t $thread --view $view > evaluation_logs/$version/view$view/$thread.log
    done

    python parse.py --logs_dir evaluation_logs/$version/view$view --output_dir evaluation_logs/$version/view$view
}

# for evaluation of the mandelbrot program in original version
# run_mandelbrot "original" "1"
# run_mandelbrot "original" "2"

# for evaluation of the mandelbrot program in improved version
run_mandelbrot "improved" "1"
run_mandelbrot "improved" "2"