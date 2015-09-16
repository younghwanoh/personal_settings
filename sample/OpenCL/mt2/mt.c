#include <stdlib.h>
#include <CL/cl.h>
#include <stdio.h>
#include <math.h>

typedef struct mt_struct_s {
	cl_uint aaa;
	cl_int mm, nn, rr, ww;
	cl_uint wmask, umask, lmask;
	cl_int shift0, shift1, shiftB, shiftC;
	cl_uint maskB, maskC;
} mt_struct;

#include "mts.h"

#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char **argv)
{
	cl_int num_rand_all = 4096*256*32; /* 생성할 난수의 개수 */
	cl_int num_rand_per_compute_unit, num_rand_per_generator;
	int count_all, i;
	cl_uint num_generator;
	unsigned int num_work_item;
	double pi;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_platforms;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel_mt = NULL, kernel_pi = NULL;
	size_t kernel_code_size;
	char *kernel_src_str;
	cl_uint *result;
	cl_int ret;
	FILE *fp;
	cl_mem rand, count;

	size_t global_item_size_mt[3], local_item_size_mt[3];
	size_t global_item_size_pi[3], local_item_size_pi[3];

	cl_mem dev_mts;
	cl_event ev_mt_end, ev_pi_end, ev_copy_end;
	cl_ulong prof_start, prof_mt_end, prof_pi_end, prof_copy_end;
	cl_uint num_compute_unit, warp_per_compute_unit, num_warp, num_param_per_warp;
	int mts_size;
	mt_struct *mts = NULL;

	if(argc >= 2) {
	   int n = atoi(argv[1]);
	   if(n == 128) {
	      mts = mts128;
	      mts_size = sizeof(mts128);
	      num_generator = 128;
	      num_rand_per_generator = num_rand_all / 128;
	   }
	   if(n == 256) {
	      mts = mts256;
	      mts_size = sizeof(mts256);
	      num_generator = 256;
	      num_rand_per_generator = num_rand_all / 256;
	   }
	}

	if(mts == NULL) {
	   mts = mts512;
	   mts_size = sizeof(mts512);
	   num_generator = 512;
	   num_rand_per_generator = num_rand_all / 512;
	}

	clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	result = (cl_uint*)malloc(sizeof(cl_uint)*num_generator);

	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);

	fp = fopen("mt.cl", "r");
	kernel_src_str = (char*)malloc(MAX_SOURCE_SIZE);
	kernel_code_size = fread(kernel_src_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 출력 버퍼를 생성 */
	rand = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint)*num_rand_all, NULL, &ret);
	count = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint)*num_generator, NULL, &ret);

	/* 프로그램 빌드 */
	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_src_str, (const size_t *)&kernel_code_size, &ret);
	clBuildProgram(program, 1, &device_id, "", NULL, NULL);

	kernel_mt = clCreateKernel(program, "genrand", &ret);
	kernel_pi = clCreateKernel(program, "calc_pi", &ret);

	/* 입력 파라미터 생성 */
	dev_mts = clCreateBuffer(context, CL_MEM_READ_WRITE, mts_size, NULL, &ret);
	clEnqueueWriteBuffer(command_queue, dev_mts, CL_TRUE, 0, mts_size, mts, 0, NULL, NULL);

	clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &num_compute_unit, NULL);

	/* 각 compute unit당 4 워프로 설정 */
	warp_per_compute_unit = 4;
	/* 워프 수 = 각 compute unit당 워프 수 * compute unit 수 */
	num_warp = warp_per_compute_unit * num_compute_unit ;
	/* 각 워프당 생성 파라미터 수(반올림 후) */
	num_param_per_warp = (num_generator + (num_warp-1)) / num_warp;
	/* 각 그룹당 워크 아이템 수(워프는 32 워크 아이템으로 설정) */
	num_work_item = 32 * warp_per_compute_unit;
	/* 각 그룹당 난수의 개수 */
	num_rand_per_compute_unit = (num_rand_all + (num_compute_unit-1)) / num_compute_unit;

	/* 커널 파라미터 설정 */
	clSetKernelArg(kernel_mt, 0, sizeof(cl_mem), (void*)&rand); /* 난수 값(genrand의 출력) */
	clSetKernelArg(kernel_mt, 1, sizeof(cl_mem), (void*)&dev_mts); /* 생성 파라미터(genrand의 입력) */
	clSetKernelArg(kernel_mt, 2, sizeof(num_rand_per_generator), &num_rand_per_generator); /* 생성 파라미터당 난수의 개수 */
	clSetKernelArg(kernel_mt, 3, sizeof(num_generator), &num_generator); /* 생성할 난수의 개수 */
	clSetKernelArg(kernel_mt, 4, sizeof(num_param_per_warp), &num_param_per_warp); /* 워크 그룹당 처리할 파라미터 수 */
	clSetKernelArg(kernel_mt, 5, sizeof(cl_uint)*17*num_work_item, NULL); /* 로컬 메모리(state) */
	clSetKernelArg(kernel_mt, 6, sizeof(mt_struct)*num_work_item, NULL); /* 로컬 메모리(mts) */

	clSetKernelArg(kernel_pi, 0, sizeof(cl_mem), (void*)&count); /* 원 안에 포함된 점의 카운터(calc_pi의 출력) */
	clSetKernelArg(kernel_pi, 1, sizeof(cl_mem), (void*)&rand); /* 난수 값(calc_pi의 입력) */
	clSetKernelArg(kernel_pi, 2, sizeof(num_rand_per_compute_unit), &num_rand_per_compute_unit); /* 각 워크 그룹이 처리할 난수의 개수 */
	clSetKernelArg(kernel_pi, 3, sizeof(num_compute_unit), &num_compute_unit); /* 생성 파라미터 수 */
	clSetKernelArg(kernel_pi, 4, sizeof(num_rand_all), &num_rand_all); /* 사용된 난수의 개수 */
	clSetKernelArg(kernel_pi, 5, sizeof(cl_uint)*128, NULL); /* 카운터로 사용된 메모리 */

	/* 워크 그룹 수와 워크 그룹당 워크 아이템 수를 설정 */
	global_item_size_mt[0] = num_work_item * num_compute_unit;
	global_item_size_mt[1] = 1;
	global_item_size_mt[2] = 1;
	local_item_size_mt[0] = num_work_item;
	local_item_size_mt[1] = 1;
	local_item_size_mt[2] = 1;

	/* 난수 배열을 생성 */
	clEnqueueNDRangeKernel(command_queue, kernel_mt, 1, NULL, global_item_size_mt, local_item_size_mt, 0, NULL, &ev_mt_end);

	global_item_size_pi[0] = num_compute_unit * 128;
	global_item_size_pi[1] = 1;
	global_item_size_pi[2] = 1;
	local_item_size_pi[0] = 128;
	local_item_size_pi[1] = 1;
	local_item_size_pi[2] = 1;

	/* PI를 계산 */
	clEnqueueNDRangeKernel(command_queue, kernel_pi, 1, NULL, global_item_size_pi, local_item_size_pi, 0, NULL, &ev_pi_end);

	/* 결과를 얻음 */
	clEnqueueReadBuffer(command_queue, count, CL_TRUE, 0, sizeof(cl_uint)*num_generator, result, 0, NULL, &ev_copy_end);

	/* PI의 평균값을 구하기 */
	count_all = 0;
	for(i = 0; i < (int)num_compute_unit; i++) {
		count_all += result[i];
	}

	pi = ((double)count_all)/(num_rand_all) * 4;
	printf("pi = %.20f\n", pi);

	/* 실행시간 정보를 얻음 */
	clGetEventProfilingInfo(ev_mt_end, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &prof_start, NULL);
	clGetEventProfilingInfo(ev_mt_end, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &prof_mt_end, NULL);
	clGetEventProfilingInfo(ev_pi_end, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &prof_pi_end, NULL);
	clGetEventProfilingInfo(ev_copy_end, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &prof_copy_end, NULL);
	
	printf(" mt: %f[ms]\n"
		   " pi: %f[ms]\n"
		   " copy: %f[ms]\n",
		   (prof_mt_end-prof_start)/(1000000.0),
		   (prof_pi_end-prof_mt_end)/(1000000.0),
		   (prof_copy_end-prof_pi_end)/(1000000.0));

	clReleaseEvent(ev_mt_end);
	clReleaseEvent(ev_pi_end);
	clReleaseEvent(ev_copy_end);

	clReleaseMemObject(rand);
	clReleaseMemObject(count);
	clReleaseKernel(kernel_mt);
	clReleaseKernel(kernel_pi);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
	free(kernel_src_str);
	free(result);
	return 0;
}