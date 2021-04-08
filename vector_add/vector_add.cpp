#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <exception>

#include <math.h>

#define __CL_ENABLE_EXCEPTIONS

#if __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/opencl.h>
#endif

#include <CL/cl.hpp>
#include <iostream>

//int main(){
//extern "C" int vector_add(){
//extern "C" int vector_add (int* A, int* B, int* C, int dimension){

struct vector_arg {
	uint32_t len;
	uint8_t *buf;
};

extern "C" int vector_add (void*out_args, size_t out_nargs, void* in_args, size_t in_nargs){
	try{
		/* unpack arguments */
		struct vector_arg *in_arg = (struct vector_arg*)in_args;
		struct vector_arg *out_arg = (struct vector_arg*) out_args;

		int dimension = *(int*)out_arg[2].buf;
		int *A = (int*)out_arg[0].buf;
		int *B = (int*)out_arg[1].buf;
		int *C = (int*)in_arg[0].buf;

		//get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if (all_platforms.size() == 0){
			std::cout << " No platforms found. Check OpenCL installation!\n";
			exit(1);
		}
		cl::Platform default_platform = all_platforms[0];
		std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

		//get default device of the default platform
		std::vector<cl::Device> all_devices;
		default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if (all_devices.size() == 0){
			std::cout << " No devices found. Check OpenCL installation!\n";
			exit(1);
		}
		cl::Device default_device = all_devices[0];
		std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

		cl::Context context({ default_device });

		cl::Program::Sources sources;
		// kernel calculates for each element C=A+B
		std::string kernel_code =
			"__kernel void simple_add(__global const int* A, __global const int* B, __global int* C) {"
			"	int index = get_global_id(0);"
			"	C[index] = A[index] + B[index];"
			"};";
		sources.push_back({ kernel_code.c_str(), kernel_code.length() });

		cl::Program program(context, sources);
		try {
			program.build({ default_device });
		}
		catch (cl::Error err) {
			std::cout << " Error building: " << 
				program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
			exit(1);
		}

		// create buffers on the device
		cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * dimension);
		cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * dimension);
		cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * dimension);


		//create queue to which we will push commands for 	the device.
		cl::CommandQueue queue(context, default_device);

		//write arrays A and B to the device
		queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * dimension, A);
		queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * dimension, B);

		cl::Kernel kernel(program, "simple_add");

		kernel.setArg(0, buffer_A);
		kernel.setArg(1, buffer_B);
		kernel.setArg(2, buffer_C);
		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(dimension), cl::NullRange);

		//int C[dimension];
		//read result C from the device to array C
		queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * dimension, C);
		queue.finish();

		//printf("address: %#llx\n", (unsigned long)C);
		std::cout << " result: \n";
		for (int i = 0; i < dimension; i++){
			std::cout << C[i] << " ";
		}
		std::cout << std::endl;
	}
	catch (cl::Error err)
	{
		printf("Error: %s (%d)\n", err.what(), err.err());
		return -1;
	}

    return 0;
}	
