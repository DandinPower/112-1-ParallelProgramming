#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

const int group_x_size = 1;
const int group_y_size = 1;

/**
 * Allocates device memory using cudaMalloc and checks for errors.
 *
 * @param devPtr Pointer to the device memory to be allocated.
 * @param size   Size of the memory to be allocated.
 */
void safe_cuda_malloc(void** devPtr, size_t size) {
    cudaError_t err = cudaMalloc(devPtr, size);
    if (err != cudaSuccess) {
        printf("Error: %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

/**
 * Calculates the number of iterations required for the given complex number to escape the Mandelbrot set.
 *
 * @param c_re The real part of the complex number.
 * @param c_im The imaginary part of the complex number.
 * @param count The maximum number of iterations to perform.
 * @return The number of iterations required for the complex number to escape the Mandelbrot set.
 */
__device__ static inline int mandel(float c_re, float c_im, int count) {
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i) {
        if (z_re * z_re + z_im * z_im > 4.f) break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    return i;
}

__global__ void mandelKernel(int *dev_output, float stepX, float stepY, float lowerX, float lowerY, int maxIterations, int pad, int resX, int resY) {
    // To avoid error caused by the floating number, use the following pseudo
    // code
    // int thisX = blockIdx.x * blockDim.x + threadIdx.x;
    // int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    int thread_x = (blockIdx.x * blockDim.x + threadIdx.x) * group_x_size;
    int thread_y = (blockIdx.y * blockDim.y + threadIdx.y) * group_y_size;

    for (int i = 0; i < group_x_size; i++) {
        for (int j = 0; j < group_y_size; j++) {
            int thisX = thread_x + i;
            int thisY = thread_y + j;

            float x = lowerX + thisX * stepX;
            float y = lowerY + thisY * stepY;
            int answer = mandel(x, y, maxIterations);

            int index = thisY * gridDim.x * blockDim.x * group_x_size + thisX;

            if (thisX >= resX || thisY >= resY) {
                continue;
            }
            if (index >= resX * resY) {
                continue;
            }

            index += (index / resX) * pad;
            dev_output[index] = answer;
        }

    }
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE(float upperX, float upperY, float lowerX, float lowerY, int* img,
            int resX, int resY, int maxIterations) {
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;

    // Allocate memory for the output image
    int* output;
    cudaHostAlloc((void**)&output, resX * resY * sizeof(int), cudaHostAllocDefault);
    // Allocate memory on the Cuda device
    int* dev_output;
    size_t pitch;
    cudaMallocPitch((void**)&dev_output, &pitch, resX * sizeof(int), resY);

    // define the number of threads per block and the number of blocks
    dim3 threads_per_block(20, 20);
    dim3 num_blocks(((resX/group_x_size) + threads_per_block.x - 1) / threads_per_block.x, ((resY/group_y_size) + threads_per_block.y - 1) / threads_per_block.y);

    // launch the kernel
    mandelKernel<<<num_blocks, threads_per_block>>>(dev_output, stepX, stepY, lowerX, lowerY, maxIterations, (pitch/sizeof(int)) - resX, resX, resY);

    // wait for the kernel to finish
    // cudaDeviceSynchronize();

    // copy the output back to host
    cudaMemcpy2D(output, resX * sizeof(int), dev_output, pitch, resX * sizeof(int), resY, cudaMemcpyDeviceToHost);

    // printf(
    //     "Total number of iterations: %d\n", output[0]
    // );

    // copy the output to the image
    memcpy(img, output, resX * resY * sizeof(int));
}
