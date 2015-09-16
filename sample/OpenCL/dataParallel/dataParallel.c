#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void CL_CALLBACK callback(cl_event evnt, cl_int cmd_sts, void *user_data);

void *work(void *data){
  int *a = (int *) data;
  unsigned int cnt=0;
  while(1){
    cnt++;
    printf("%d\n", *a);
    if(*a == 1){
      printf("%d\n", *a);
      printf("\n!!finish at %d!!\n", cnt);
      break;
    }
  }
  return;
}

int main()
{
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem Amobj = NULL;
	cl_mem Bmobj = NULL;
	cl_mem Cmobj = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
  cl_event evnt1;
  int a = 0;
  pthread_t thread_t;

  unsigned int cnt = 0;
	int i, j;
	float *A;
	float *B;
	float *C;

	A = (float *)malloc(4*4*sizeof(float));
	B = (float *)malloc(4*4*sizeof(float));
	C = (float *)malloc(4*4*sizeof(float));

	FILE *fp;
	const char fileName[] = "./dataParallel.cl";
	size_t source_size;
	char *source_str;

	/* 커널을 포함한 소스 코드를 로드 */
	fp = fopen(fileName, "r");
	if(!fp) {
	fprintf(stderr, "Failed to load kernel.\n");
	exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 입력 데이터 초기화 */
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			A[i*4+j] = i*4+j+1;
			B[i*4+j] = j*4+i+1;
		}
	}

	/* 플랫폼, 디바이스 정보를 얻음 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* OpenCL 컨텍스트 생성 */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* 버퍼 오브젝트 생성 */
	Amobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4*4*sizeof(float), NULL, &ret);
	Bmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4*4*sizeof(float), NULL, &ret);
	Cmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4*4*sizeof(float), NULL, &ret);

	/* 메모리 버퍼에 입력 데이터를 복사 */
	ret = clEnqueueWriteBuffer(command_queue, Amobj, CL_TRUE, 0, 4*4*sizeof(float), A, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, Bmobj, CL_TRUE, 0, 4*4*sizeof(float), B, 0, NULL, NULL);

	/* 미리 로드한 소스 코드로 커널 프로그램을 생성 */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* 데이터 병렬 작업을 실행하는 OpenCL 커널 생성 */
	kernel = clCreateKernel(program, "dataParallel", &ret);

	/* OpenCL 커널 파라미터 설정 */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&Amobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&Bmobj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&Cmobj);

	size_t global_item_size = 4;
	size_t local_item_size = 1;

	/* OpenCL 커널로 데이터 병렬을 실행 */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, &evnt1);
  ret = clSetEventCallback(evnt1, CL_COMPLETE, callback, (void *)& a);


  pthread_create(&thread_t, NULL, work, (void *)&a);
//  pthread_join(thread_t, NULL);

	/* 결과를 호스트에 전송 */
	ret = clEnqueueReadBuffer(command_queue, Cmobj, CL_TRUE, 0, 4*4*sizeof(float), C, 0, NULL, NULL);

	/* 결과 출력 */
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			printf("%7.2f ", C[i*4+j]);
		}
		printf("\n");
	}

//  printf("result: %d\n" , a);

	/* OpenCL 오브젝트 종료 처리 */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(Amobj);
	ret = clReleaseMemObject(Bmobj);
	ret = clReleaseMemObject(Cmobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);

	free(A);
	free(B);
	free(C);

	return 0;
}

void callback(cl_event evnt, cl_int cmd_sts, void *user_data){

  int* k = (int *)user_data;
  *k = 1;
  printf("\n %d callback ! \n", *k);
}
