/*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2012  Aubry Adrien (dridri85)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ge_internal.h"
char* load_source(const char* file, int* size);

static bool checkVal(cl_int status, int value, const char* err_str){
	if((int)status == value){
		return true;
	}else{
		printf("GE::CL::%s\n", err_str);
		return false;
	}
}

cl_platform_id ge_ClGetPlatform(ge_Cl* cl){
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	cl_int status;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if(!checkVal(status, CL_SUCCESS, "clGetPlatformIDs failed.")){
		return (cl_platform_id)-1;
	}
	if(0 < numPlatforms){
		cl_platform_id* platforms = (cl_platform_id*)geMalloc(sizeof(cl_platform_id)*numPlatforms);
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if(!checkVal(status, CL_SUCCESS, "clGetPlatformIDs failed.")){
			return (cl_platform_id)-1;
		}
		
		char platformVendor[100];
		char platformName[100];
		u32 i = 0;
		for(i=0; i < numPlatforms; ++i) {
			status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
			status = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
			if(!checkVal(status, CL_SUCCESS, "clGetPlatformInfo failed.")){
				return (cl_platform_id)-1;
			}
			platform = platforms[i];
			if(!strcmp(platformVendor, "Advanced Micro Devices, Inc.") || !strcmp(platformVendor, "NVIDIA")){
				gePrintDebug(0x100, "CL::Using platform '%s'\n", platformName);
				break;
			}
		}
		free(platforms);
	}
	return platform;
}

ge_Cl* geCreateCl(const char* name, int preferred_type, const char* file){
	ge_Cl* cl = (ge_Cl*)geMalloc(sizeof(ge_Cl));
	
	cl_platform_id platform = (cl_platform_id)-1;
	cl_device_id* devices;
	cl_uint numDevices = 0;
	int deviceUsed = -1;

	printf("\nGE::CL::Initialize OpenCL object and context\n");
	platform = ge_ClGetPlatform(cl);
	printf("GE::CL::PlatformID: %d\n", (u32)platform);
	cl->err = clGetDeviceIDs(platform, preferred_type, 0, NULL, &numDevices);
	printf("GE::CL::clGetDeviceIDs (get number of devices): %d\n", numDevices);
	devices = (cl_device_id*)geMalloc(sizeof(cl_device_id)*numDevices);
	cl->err = clGetDeviceIDs(platform, preferred_type, numDevices, devices, NULL);
	printf("GE::CL::clGetDeviceIDs (create device list): %d\n", cl->err);
	cl->context = clCreateContext(0, 1, devices, NULL, NULL, &cl->err);
	deviceUsed = 0;
	cl->command_queue = clCreateCommandQueue(cl->context, devices[deviceUsed], 0, &cl->err);
	
	int size = 0;
	char* source = load_source(file, NULL);
	cl->program = clCreateProgramWithSource(cl->context, 1, (const char **)&source, (const size_t*)&size, &cl->err);

	printf("GE::CL::building the program\n");
	cl->err = clBuildProgram(cl->program, 0, NULL, NULL, NULL, NULL);
	printf("GE::CL::clBuildProgram: %d\n", cl->err);
	cl_build_status build_status;
	cl->err = clGetProgramBuildInfo(cl->program, devices[deviceUsed], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
	char* build_log;
	size_t ret_val_size;
	cl->err = clGetProgramBuildInfo(cl->program, devices[deviceUsed], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
	build_log = (char*)geMalloc(ret_val_size+1);
	cl->err = clGetProgramBuildInfo(cl->program, devices[deviceUsed], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
	build_log[ret_val_size] = 0x0;
	printf("GE::CL::BUILD LOG: \n %s\n\n", build_log);
	printf("GE::CL::program built\n");

	clCreateKernelsInProgram(cl->program, 16, cl->kernels, &cl->nKernels);
//	cl->kernels[0] = clCreateKernel(cl->program, "MatrixMult", &cl->err);

	return cl;
}

cl_mem geClCreateBuffer(ge_Cl* cl, int mem_mode, int size, void* data){
	return clCreateBuffer(cl->context, mem_mode, size, data, &cl->err);
}

void geClBufferWrite(ge_Cl* cl, cl_mem id, int offset, size_t size, void* arg){
	clEnqueueWriteBuffer(cl->command_queue, id, CL_TRUE, offset, size, arg, 0, NULL, &cl->event);
	clReleaseEvent(cl->event);
}

void geClBufferRead(ge_Cl* cl, cl_mem id, int offset, size_t size, void* arg){
	cl->err = clEnqueueReadBuffer(cl->command_queue, id, CL_TRUE, offset, size, arg, 0, NULL, &cl->event);
	clReleaseEvent(cl->event);
}

void geClArg(ge_Cl* cl, int kernel_index, int index, void* arg, size_t size){
	clSetKernelArg(cl->kernels[kernel_index], index, size, arg);
	clFinish(cl->command_queue);
}

void geClExecute(ge_Cl* cl, int kernel_index, int nWG, int nWI){
	size_t nWorkGroups[1], nWorkItems[1];
	nWorkGroups[0] = nWG;
	nWorkItems[0] = nWI;
	cl_int ret = clEnqueueNDRangeKernel(cl->command_queue, cl->kernels[kernel_index], 1, NULL, nWorkGroups, nWorkItems, 0, NULL, &cl->event);
	clReleaseEvent(cl->event);
	gePrintDebug(0x100, "clEnqueueNDRangeKernel returned %d\n", ret);
}

void geClSync(ge_Cl* cl){
	clFinish(cl->command_queue);
}
