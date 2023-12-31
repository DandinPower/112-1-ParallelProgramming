__kernel void convolution(__global const float* filter, const int filterWidth,
                          __global const float* inputImage, __global float* outputImage,
                          const int imageWidth, const int imageHeight, __local float* localFilter) {
    event_t e1 = async_work_group_copy(localFilter, filter, filterWidth * filterWidth, 0);
    
    int x = get_global_id(0); // col
    int y = get_global_id(1); // row
    int halffilterSize = filterWidth / 2;    
    float sum = 0.0;
    int k, l;

    wait_group_events(1, &e1);

    for (k = -halffilterSize; k <= halffilterSize; k++) {
        for (l = -halffilterSize; l <= halffilterSize; l++) {
            if (y + k >= 0 && y + k < imageHeight && x + l >= 0 && x + l < imageWidth) {
                sum += inputImage[(y + k) * imageWidth + x + l] * filter[(k + halffilterSize) * filterWidth + l + halffilterSize];
            }
        }
    }
    outputImage[y * imageWidth + x] = sum;
}