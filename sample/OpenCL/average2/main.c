#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <stdio.h>

/* 주가 데이터를 읽어들이기 */
int stock_array1[] = {
	#include "stock_array1.txt"
};

/* 이동평균 폭 설정 */
#define WINDOW_SIZE (13)

#define MAX_SOURCE_SIZE (0x100000)

int main(void)
{
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_platforms;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobj_in = NULL;
	cl_mem memobj_out = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	size_t kernel_code_size;
	char *kernel_src_str;
	float *result;
	cl_int ret;
	FILE *fp;

	int data_num = sizeof(stock_array1) / sizeof(stock_array1[0]);
	int window_num = (int)WINDOW_SIZE;
	int i;

	/* 커널 프로그램을 읽어들일 메모리를 할당 */
	kernel_src_str = (char *)malloc(MAX_SOURCE_SIZE);

	/* 처리 결과를 저장할 메모리를 호스트에 할당 */
	result = (float *)malloc(data_num*sizeof(float));

	/* 플랫폼 정보 얻기 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	/* 디바이스 정보 얻기 */
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* 컨텍스트 생성 */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* 커널 프로그램 읽어들이기 */
	fp = fopen("moving_average.cl", "r");
	kernel_code_size = fread(kernel_src_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 프로그램 오브젝트를 생성 */
	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_src_str, (const size_t *)&kernel_code_size, &ret);
	/* 커널 컴파일 */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* 커널 생성 */
	kernel = clCreateKernel(program, "moving_average", &ret);

	/* 입력 데이터를 전달하기 위한 메모리를 디바이스에 생성 */
	memobj_in = clCreateBuffer(context, CL_MEM_READ_WRITE, data_num * sizeof(int), NULL, &ret);

	/* 처리 결과를 저장하기 위한 메모리를 디바이스에 생성 */
	memobj_out = clCreateBuffer(context, CL_MEM_READ_WRITE, data_num * sizeof(float), NULL, &ret);

	/* 입력 데이터를 디바이스의 글로벌 메모리에 전송 */
	ret = clEnqueueWriteBuffer(command_queue, memobj_in, CL_TRUE, 0, data_num * sizeof(int), stock_array1, 0, NULL, NULL);

	/* 커널 파라미터 설정 */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobj_out);
	ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&data_num);
	ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&window_num);

	/* 커널 실행 */
	ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

	/* 처리 결과를 디바이스에서 호스트로 전송 */
	ret = clEnqueueReadBuffer(command_queue, memobj_out, CL_TRUE, 0, data_num * sizeof(float), result, 0, NULL, NULL);


	/* OpenCL 오브젝트의 종료 처리 */
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj_in);
	ret = clReleaseMemObject(memobj_out);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	/* 처리 결과를 출력 */
	for(i = 0; i < data_num; i++) {
		printf("result[%d] = %f\n", i, result[i]);
	}

	/* 호스트에 할당했던 메모리를 해제 */
	free(result);
	free(kernel_src_str);

	return 0;
}