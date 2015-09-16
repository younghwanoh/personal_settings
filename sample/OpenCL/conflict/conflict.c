#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MEM_SIZE (10)
#define MAX_SOURCE_SIZE (0x100000)

int main()
{
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	cl_mem StrMemObj = NULL;
	cl_mem LOGMemObj = NULL;
	cl_mem ConditionMemObj = NULL;

  int i;
//  int init[] = {1,2,3,4,5,6,7,8,9,10};
//  int initLOG[10] = {0,};
	char string[MEM_SIZE];
  int *AccessLOG = (int *) malloc(MEM_SIZE*sizeof(int));
  int *condVar = (int *) malloc(MEM_SIZE*sizeof(int)); 

  for (i=0;i<10;i++){
    condVar[i] = 1;
    AccessLOG[i] = 0;
  }
  //condVar = init;

	FILE *fp;
	char fileName[] = "./kernel_conflict.cl";
	char *source_str;
	size_t source_size;

	/* 커널을 포함한 소스 코드를 로드 */
	fp = fopen(fileName, "r");
	if(!fp) {
	   fprintf(stderr, "Failed to load kernel.\n");
	   exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 플랫폼, 디바이스 정보를 얻음 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	/* OpenCL 컨텍스트 생성 */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* 메모리 버퍼 생성 */
	StrMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &ret);
  ConditionMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(int), NULL, &ret);
	LOGMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(int), NULL, &ret);

  /* 버퍼 오브젝트 생성*/
	ret = clEnqueueWriteBuffer(command_queue, LOGMemObj, CL_TRUE, 0, MEM_SIZE * sizeof(int), AccessLOG, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, StrMemObj, CL_TRUE, 0, MEM_SIZE * sizeof(int), string, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, ConditionMemObj, CL_TRUE, 0, MEM_SIZE * sizeof(int), condVar, 0, NULL, NULL);

	/* 미리 로드한 소스 코드로 커널 프로그램을 생성 */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, "", NULL, NULL);

	/* OpenCL 커널 생성*/
	kernel = clCreateKernel(program, "compute", &ret);

	/* OpenCL 커널 파라미터 설정 */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&StrMemObj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&ConditionMemObj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&LOGMemObj);

	size_t global_item_size = MEM_SIZE;
	size_t local_item_size = 1;

	/* OpenCL 커널 실행 */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	/* 실행 결과를 메모리 버퍼에서 얻음 */
	ret = clEnqueueReadBuffer(command_queue, StrMemObj, CL_TRUE, 0, MEM_SIZE * sizeof(char), string, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, LOGMemObj, CL_TRUE, 0, MEM_SIZE * sizeof(int), AccessLOG, 0, NULL, NULL);

	/* 결과 출력 */
  for(i=0;i<MEM_SIZE;i++)
	  printf("%dth LOG: %d\n", i, AccessLOG[i]);
  puts(string);

	/* 종료 처리 */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(StrMemObj);
	ret = clReleaseMemObject(ConditionMemObj);
	ret = clReleaseMemObject(LOGMemObj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);

	return 0;
}
