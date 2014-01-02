#include <CL/opencl.h>
#include "getypes.h"

typedef struct ge_Cl {
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernels[16];
	cl_uint nKernels;
	cl_int err;
	cl_event event;
} ge_Cl;

LIBGE_API ge_Cl* geCreateCl(const char* name, int preferred_type, const char* file);
LIBGE_API cl_mem geClCreateBuffer(ge_Cl* cl, int mem_mode, int size, void* data);
LIBGE_API void geClBufferWrite(ge_Cl* cl, cl_mem id, int offset, size_t size, void* arg);
LIBGE_API void geClBufferRead(ge_Cl* cl, cl_mem id, int offset, size_t size, void* arg);
LIBGE_API void geClArg(ge_Cl* cl, int kernel_index, int index, void* arg, size_t size);
LIBGE_API void geClExecute(ge_Cl* cl, int kernel_index, int nWG, int nWI);
LIBGE_API void geClSync(ge_Cl* cl);
