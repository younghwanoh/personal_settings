#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <stdio.h>

#define NAME_NUM (4) /* 종목 수 */
#define DATA_NUM (100) /* 각 종목의 처리 데이터 수 */

/* 주가 데이터를 읽어들이기 */
int stock_array_4[NAME_NUM*DATA_NUM]= {
	#include "stock_array_4.txt"
};

/* 이동평균 폭 설정 */
#define WINDOW_SIZE_13 (13)
#define WINDOW_SIZE_26 (26)


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
	cl_mem memobj_out13 = NULL;
	cl_mem memobj_out26 = NULL;
	cl_program program = NULL;
	cl_kernel kernel13 = NULL;
	cl_kernel kernel26 = NULL;
	cl_event event13, event26;
	size_t kernel_code_size;
	char *kernel_src_str;
	float *result13;
	float *result26;
	cl_int ret;
	FILE *fp;

	int window_num_13 = (int)WINDOW_SIZE_13;
	int window_num_26 = (int)WINDOW_SIZE_26;
	int point_num = NAME_NUM * DATA_NUM;
	int data_num = (int)DATA_NUM;
	int name_num = (int)NAME_NUM;

	int i, j;

	/* 커널 프로그램을 읽어들일 메모리를 할당 */
	kernel_src_str = (char *)malloc(MAX_SOURCE_SIZE);

	/* 처리 결과를 저장할 메모리를 호스트에 할당 */
	result13 = (float *)malloc(point_num*sizeof(float)); /* average over 13 weeks */
	result26 = (float *)malloc(point_num*sizeof(float)); /* average over 26 weeks */

	/* 플랫폼 정보 얻기 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	/* 디바이스 정보 얻기 */
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* 컨텍스트 얻기 */
	context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);

	/* 커널 프로그램 읽어들이기 */
	fp = fopen("moving_average_vec4.cl", "r");
	kernel_code_size = fread(kernel_src_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 프로그램 오브젝트를 생성 */
	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_src_str, (const size_t *)&kernel_code_size, &ret);

	/* 커널 컴파일 */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* 커널 생성 */
	kernel13 = clCreateKernel(program, "moving_average_vec4", &ret); /* 13 weeks */
	kernel26 = clCreateKernel(program, "moving_average_vec4", &ret); /* 26 weeks */

	/* 입력 데이터를 전달하기 위한 메모리를 디바이스에 생성 */
	memobj_in = clCreateBuffer(context, CL_MEM_READ_WRITE, point_num * sizeof(int), NULL, &ret);

	/* 처리 결과를 저장하기 위한 메모리를 디바이스에 생성 */
	memobj_out13 = clCreateBuffer(context, CL_MEM_READ_WRITE, point_num * sizeof(float), NULL, &ret); /* 13 weeks */
	memobj_out26 = clCreateBuffer(context, CL_MEM_READ_WRITE, point_num * sizeof(float), NULL, &ret); /* 26 weeks */

	/* 입력 데이터를 디바이스의 글로벌 메모리에 전송 */
	ret = clEnqueueWriteBuffer(command_queue, memobj_in, CL_TRUE, 0, point_num * sizeof(int), stock_array_4, 0, NULL, NULL);

	/* 커널 파라미터 설정 (13주) */
	ret = clSetKernelArg(kernel13, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel13, 1, sizeof(cl_mem), (void *)&memobj_out13);
	ret = clSetKernelArg(kernel13, 2, sizeof(int), (void *)&data_num);
	ret = clSetKernelArg(kernel13, 3, sizeof(int), (void *)&window_num_13);

	/* 13주 이동평균 커널을 실행 */
	ret = clEnqueueTask(command_queue, kernel13, 0, NULL, &event13);

	/* 커널 파라미터 설정 (26주) */
	ret = clSetKernelArg(kernel26, 0, sizeof(cl_mem), (void *)&memobj_in);
	ret = clSetKernelArg(kernel26, 1, sizeof(cl_mem), (void *)&memobj_out26);
	ret = clSetKernelArg(kernel26, 2, sizeof(int), (void *)&data_num);
	ret = clSetKernelArg(kernel26, 3, sizeof(int), (void *)&window_num_26);

	/* 26주 이동평균 커널을 실행 */
	ret = clEnqueueTask(command_queue, kernel26, 0, NULL, &event26);

	/* 13주 이동평균 커널 실행 결과를 디바이스에서 호스트로 전송 */
	ret = clEnqueueReadBuffer(command_queue, memobj_out13, CL_TRUE, 0, point_num * sizeof(float), result13, 1, &event13, NULL);

	/* 26주 이동평균 커널 실행 결과를 디바이스에서 호스트로 전송*/
	ret = clEnqueueReadBuffer(command_queue, memobj_out26, CL_TRUE, 0, point_num * sizeof(float), result26, 1, &event26, NULL);

	/* OpenCL 오브젝트의 종료 처리 */
	ret = clReleaseKernel(kernel13);
	ret = clReleaseKernel(kernel26);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj_in);
	ret = clReleaseMemObject(memobj_out13);
	ret = clReleaseMemObject(memobj_out26);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	/* 결과 출력 */
	for(i = window_num_26-1; i < data_num; i++) {
		printf("result[%d]:", i );
		for(j = 0; j < name_num; j++ ) {
			/* 판정결과 */
			printf( "[%d] ", (result13[i*NAME_NUM+j] > result26[i*NAME_NUM+j]) );
		}
		printf("\n");
	}

	/* 호스트에 할당했던 메모리를 해제 */
	free(result13);
	free(result26);
	free(kernel_src_str);

	return 0;
}