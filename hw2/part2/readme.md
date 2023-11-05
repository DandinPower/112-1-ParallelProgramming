```
# Installation Instructions

First, install Python3 and its related packages on your system:

```bash
sudo apt-get install python3
sudo apt-get install python3-pip
pip3 install -r requirements.txt
```

# MandelbrotThread.cpp Experiments

In the `mandelbrotThread.cpp` file, there are two different experiments:

```c++
// original version (spatial decomposition, but can't reach linear speedup)
int startRow = std::ceil(args->height * static_cast<double>(args->threadId) / args->numThreads);
int endRow = std::ceil(args->height * static_cast<double>(args->threadId + 1) / args->numThreads);
int totalRows = endRow - startRow;
mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, totalRows, args->maxIterations, args->output);

// improved version (spatial decomposition, but with by row slicing)

// for (unsigned int i = args->threadId * ROWS_PER_THREAD; i < args->height; i += (args->numThreads * ROWS_PER_THREAD)) {
//     int totalRows = std::min(args->height - i, ROWS_PER_THREAD);
//     totalRows = std::max(totalRows, 0);
//     mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, i, totalRows, args->maxIterations, args->output);
// }
```

# Evaluation.sh Experiments

The `evaluation.sh` script also includes two sets of experiments:

```bash
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
run_mandelbrot "original" "1"
run_mandelbrot "original" "2"

# for evaluation of the mandelbrot program in improved version
# run_mandelbrot "improved" "1"
# run_mandelbrot "improved" "2"
```

# Running the Evaluations

To run the different evaluations, follow the instructions below:

## Run the Original Version

1. Make sure `mandelbrot.cpp` is the original version.
2. Ensure `evaluation.sh` is set to the original version, which means the last two lines are commented.
3. Clear the compiled file by `make clean`.
4. Compile the file by `make`.
5. Run the evaluation by `bash evaluation.sh`.
6. The results will appear in the `evaluation_logs/original/view1` and `evaluation_logs/original/view2`.

## Run the Improved Version

1. Make sure `mandelbrot.cpp` is the improved version.
2. Ensure `evaluation.sh` is set to the improved version, which means the last two lines are not commented, and the first two lines are commented.
3. Clear the compiled file by `make clean`.
4. Compile the file by `make`.
5. Run the evaluation by `bash evaluation.sh`.
6. The results will appear in the `evaluation_logs/improved/view1` and `evaluation_logs/improved/view2`.
```
