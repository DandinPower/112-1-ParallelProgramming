#include "hostFE.h"
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program) {
    cl_int status;
    int filterSize = filterWidth * filterWidth;

    cl_command_queue queue =
        clCreateCommandQueue(*context, *device, 0, &status);
    CHECK(status, "create command queue");

    cl_mem filterBuffer =
        clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                       sizeof(float) * filterSize, filter, &status);
    CHECK(status, "create filter buffer");

    cl_mem inputBuffer = clCreateBuffer(
        *context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * imageHeight * imageWidth, inputImage, &status);
    CHECK(status, "create input buffer");

    cl_mem outputBuffer =
        clCreateBuffer(*context, CL_MEM_WRITE_ONLY,
                       sizeof(float) * imageHeight * imageWidth, NULL, &status);
    CHECK(status, "create output buffer");

    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    CHECK(status, "create kernel");

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &filterBuffer);
    clSetKernelArg(kernel, 1, sizeof(int), &filterWidth);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 4, sizeof(int), &imageWidth);
    clSetKernelArg(kernel, 5, sizeof(int), &imageHeight);
    clSetKernelArg(kernel, 6, filterWidth * filterWidth * sizeof(float), NULL);

    size_t globalWorkSize[2] = {imageWidth, imageHeight};
    status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize,
                                    NULL, 0, NULL, NULL);
    CHECK(status, "enqueue kernel");

    status = clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0,
                                 sizeof(float) * imageHeight * imageWidth,
                                 outputImage, 0, NULL, NULL);
    CHECK(status, "read output buffer");

    clReleaseCommandQueue(queue);
    clReleaseKernel(kernel);
    clReleaseMemObject(filterBuffer);
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
}