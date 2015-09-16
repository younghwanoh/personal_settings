#include <cassert>
#include <iostream>
#include <CL/cl.h>

int main(int argc, char *argv[])
{
	cl_int ret;

	/* 플랫폼 ID를 얻음 */
	cl_uint num_platform;
	cl_platform_id platform_id;
	ret = clGetPlatformIDs(1, &platform_id, &num_platform);
	assert(CL_SUCCESS == ret);

	/* 디바이스 ID를 얻음 */
	cl_uint num_device;
	cl_device_id device_ids[2];
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 2, device_ids, &num_device);
	assert(CL_SUCCESS == ret);

	/* 컨텍스트, 커맨드 큐 생성 */
	cl_context context;
	cl_command_queue command_queues[2];
	context = clCreateContext(NULL, 2, &device_ids[0], NULL, NULL, &ret);
	assert(CL_SUCCESS == ret);

	for(unsigned int i = 0; i < 2; ++i) {
		command_queues[i] = clCreateCommandQueue(context, device_ids[i], 0, &ret);
		assert(CL_SUCCESS == ret);
	}

	cl_mem mem1 = clCreateBuffer(context, CL_MEM_READ_WRITE, 128*sizeof(cl_float4), NULL, &ret);
	assert(CL_SUCCESS == ret);

	ret = clEnqueueMigrateMemObjects(command_queues[0], 1, &mem1, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0, 0, 0);
	assert(CL_SUCCESS == ret);

	cl_mem mem2 = clCreateBuffer(context, CL_MEM_READ_WRITE, 128*sizeof(cl_float4), NULL, &ret);
	assert(CL_SUCCESS == ret);

	ret = clEnqueueMigrateMemObjects(command_queues[1], 1, &mem2, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0, 0, 0);
	assert(CL_SUCCESS == ret);

	clReleaseMemObject(mem1);
	clReleaseMemObject(mem2);

	for(unsigned int i = 0; i < 2; ++i) {
		clReleaseCommandQueue(command_queues[i]);
	}

	clReleaseContext(context);

	return 0;
}