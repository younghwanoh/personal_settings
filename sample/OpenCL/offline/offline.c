#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MEM_SIZE (128)
#define MAX_BINARY_SIZE (0x100000)

int main()
{
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	float mem[MEM_SIZE];

	FILE *fp;
	char fileName[] = "./kernel.o";
	size_t binary_size;
	char *binary_buf;
	cl_int binary_status;
	cl_int i;

	/* 이미 빌드한 커널 바이너리를 로드 */
	fp = fopen(fileName, "r");
	if(!fp) {
	   fprintf(stderr, "Failed to load kernel.\n");
	   exit(1);
	}
	binary_buf = (char *)malloc(MAX_BINARY_SIZE);
	binary_size = fread(binary_buf, 1, MAX_BINARY_SIZE, fp);
	fclose(fp);

	/* 입력 데이터 초기화 */
	for(i = 0; i < MEM_SIZE; i++) {
	    mem[i] = i;
	}

	/* 플랫폼, 디바이스 정보를 얻음 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* OpenCL 컨텍스트 생성 */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* 메모리 버퍼 생성 */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(float), NULL, &ret);

	/* 메모리 버퍼로 데이터를 전송 */
	ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, MEM_SIZE * sizeof(float), mem, 0, NULL, NULL);

	/* 미리 로드한 커널 바이너리로 커널 프로그램을 생성 */
	program = clCreateProgramWithBinary(context, 1, &device_id, (const size_t *)&binary_size, (const unsigned char **)&binary_buf, &binary_status, &ret);

	/* OpenCL 커널 생성 */
	kernel = clCreateKernel(program, "vecAdd", &ret);
	printf("err:%d\n", ret);

	/* OpenCL 커널 파라미터 설정 */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);

	size_t global_work_size[3] = {MEM_SIZE, 0, 0};
	size_t local_work_size[3] = {MEM_SIZE, 0, 0};

	/* OpenCL 커널 실행 */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,　global_work_size, local_work_size, 0, NULL, NULL);

	/* 메모리 버퍼로부터 실행 결과를 얻음 */
	ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, MEM_SIZE * sizeof(float), mem, 0, NULL, NULL);

	/* 결과 출력 */
	for(i = 0; i < MEM_SIZE; i++) {
	    printf("mem[%d] :%f\n", i, mem[i]);
	}

	/* OpenCL 오브젝트의 종료 처리 */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(binary_buf);

	return 0;
}
