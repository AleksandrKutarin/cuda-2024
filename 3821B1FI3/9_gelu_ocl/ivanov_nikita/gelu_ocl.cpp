// Copyright (c) 2024 Ivanov Nikita
#include "gelu_ocl.h"
#include <CL/opencl.hpp>
#include <iostream>
#include <vector>
#include <utility>


std::vector<float> GeluOCL(const std::vector<float>& input) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();

    cl::Context context(device);
    cl::CommandQueue queue(context);

    std::string kernel_source = R"(
__kernel void myGelu(__global const float* input, __global float* output, int size) {
    int idx = get_global_id(0);
    if (idx < size) {
        float x = input[idx];
        output[idx] = x / (1.0f + exp(-1.59577f * (x + 0.044715f * x * x * x)));
    }
}
)";

    cl::Program::Sources sources;
    sources.emplace_back(std::move(kernel_source));

    cl::Program program(context, sources);
    program.build();

    cl::Kernel kernel(program, "myGelu");

    size_t size = input.size();
    size_t sizeInBytes = size * sizeof(*input.data());
    
    std::vector<float> output(size);

    cl::Buffer input_buffer(context, CL_MEM_READ_ONLY, sizeInBytes);
    cl::Buffer output_buffer(context, CL_MEM_WRITE_ONLY, sizeInBytes);

    queue.enqueueWriteBuffer(input_buffer, CL_TRUE,
                             0, sizeInBytes,
                             input.data());

    kernel.setArg(0, input_buffer);
    kernel.setArg(1, output_buffer);
    kernel.setArg(2, static_cast<int>(size));

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size), cl::NullRange);

    queue.enqueueReadBuffer(output_buffer, CL_TRUE, 0, sizeInBytes, output.data());

    return output;
}
