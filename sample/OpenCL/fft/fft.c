#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "pgm.h"

#define PI 3.14159265358979

#define MAX_SOURCE_SIZE (0x100000)

#define AMP(a, b) (sqrt((a)*(a)+(b)*(b)))

cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue queue = NULL;
cl_program program = NULL;

enum Mode {
	forward = 0,
	inverse = 1
};

int setWorkSize(size_t* gws, size_t* lws, cl_int x, cl_int y)
{
	switch(y) {
		case 1:
			gws[0] = x;
			gws[1] = 1;
			lws[0] = 1;
			lws[1] = 1;
			break;
		default:
			gws[0] = x;
			gws[1] = y;
			lws[0] = 1;
			lws[1] = 1;
			break;
	}

	return 0;
}

int fftCore(cl_mem dst, cl_mem src, cl_mem spin, cl_int m, enum Mode direction)
{
	cl_int ret;

	cl_int iter;
	cl_uint flag;

	cl_int n = 1 << m;

	cl_event kernelDone;

	cl_kernel brev = NULL;
	cl_kernel bfly = NULL;
	cl_kernel norm = NULL;

	brev = clCreateKernel(program, "bitReverse", &ret);
	bfly = clCreateKernel(program, "butterfly", &ret);
	norm = clCreateKernel(program, "norm", &ret);

	size_t gws[2];
	size_t lws[2];

	switch (direction) {
		case forward:flag = 0x00000000; break;
		case inverse:flag = 0x80000000; break;
	}

	ret = clSetKernelArg(brev, 0, sizeof(cl_mem), (void *)&dst);
	ret = clSetKernelArg(brev, 1, sizeof(cl_mem), (void *)&src);
	ret = clSetKernelArg(brev, 2, sizeof(cl_int), (void *)&m);
	ret = clSetKernelArg(brev, 3, sizeof(cl_int), (void *)&n);

	ret = clSetKernelArg(bfly, 0, sizeof(cl_mem), (void *)&dst);
	ret = clSetKernelArg(bfly, 1, sizeof(cl_mem), (void *)&spin);
	ret = clSetKernelArg(bfly, 2, sizeof(cl_int), (void *)&m);
	ret = clSetKernelArg(bfly, 3, sizeof(cl_int), (void *)&n);
	ret = clSetKernelArg(bfly, 5, sizeof(cl_uint), (void *)&flag);

	ret = clSetKernelArg(norm, 0, sizeof(cl_mem), (void *)&dst);
	ret = clSetKernelArg(norm, 1, sizeof(cl_int), (void *)&n);

	/* 비트 역순으로 뒤집기 */
	setWorkSize(gws, lws, n, n);
	ret = clEnqueueNDRangeKernel(queue, brev, 2, NULL, gws, lws, 0, NULL, NULL);

	/* 버터플라이 연산 실행 */
	setWorkSize(gws, lws, n/2, n);
	for(iter = 1; iter <= m; iter++) {
		ret = clSetKernelArg(bfly, 4, sizeof(cl_int), (void *)&iter);
		ret = clEnqueueNDRangeKernel(queue, bfly, 2, NULL, gws, lws, 0, NULL, &kernelDone);
		ret = clWaitForEvents(1, &kernelDone);
	}

	if(direction == inverse) {
	   setWorkSize(gws, lws, n, n);
	   ret = clEnqueueNDRangeKernel(queue, norm, 2, NULL, gws, lws, 0, NULL, &kernelDone);
	   ret = clWaitForEvents(1, &kernelDone);
	}

	ret = clReleaseKernel(bfly);
	ret = clReleaseKernel(brev);
	ret = clReleaseKernel(norm);

	return 0;
}

int main()
{
	cl_mem xmobj = NULL;
	cl_mem rmobj = NULL;
	cl_mem wmobj = NULL;
	cl_kernel sfac = NULL;
	cl_kernel trns = NULL;
	cl_kernel hpfl = NULL;

	cl_platform_id platform_id = NULL;

	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;

	cl_int ret;

	cl_float2 *xm;
	cl_float2 *rm;
	cl_float2 *wm;

	pgm_t ipgm;
	pgm_t opgm;

	FILE *fp;
	const char fileName[] = "./fft.cl";
	size_t source_size;
	char *source_str;
	cl_int i, j;
	cl_int n;
	cl_int m;

	size_t gws[2];
	size_t lws[2];

	/* 커널 프로그램 읽어들이기 */
	fp = fopen(fileName, "r");
	if(!fp) {
	fprintf(stderr, "Failed to load kernel.\n");
	exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* 이미지 읽어들이기 */
	readPGM(&ipgm, "lena.pgm");

	n = ipgm.width;
	m = (cl_int)(log((double)n)/log(2.0));

	xm = (cl_float2 *)malloc(n * n * sizeof(cl_float2));
	rm = (cl_float2 *)malloc(n * n * sizeof(cl_float2));
	wm = (cl_float2 *)malloc(n / 2 * sizeof(cl_float2));

	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++) {
			((float*)xm)[(2*n*j)+2*i+0] = (float)ipgm.buf[n*j+i];
			((float*)xm)[(2*n*j)+2*i+1] = (float)0;
		}
	}

	/* 플랫폼과 디바이스 정보 얻기 */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* OpenCL 컨텍스트 생성 */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* 커맨드 큐 생성 */
	queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* 버퍼 오브젝트 생성 */
	xmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float2), NULL, &ret);
	rmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float2), NULL, &ret);
	wmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, (n/2)*sizeof(cl_float2), NULL, &ret);

	/* 메모리 버퍼에 데이터를 전송 */
	ret = clEnqueueWriteBuffer(queue, xmobj, CL_TRUE, 0, n*n*sizeof(cl_float2), xm, 0, NULL, NULL);

	/* 읽어들인 커널 소스를 이용해 커널 프로그램을 생성 */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	/* 커널 프로그램 빌드 */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* OpenCL 커널 생성 */
	sfac = clCreateKernel(program, "spinFact", &ret);
	trns = clCreateKernel(program, "transpose", &ret);
	hpfl = clCreateKernel(program, "highPassFilter", &ret);

	/* 회전인자 생성 */
	ret = clSetKernelArg(sfac, 0, sizeof(cl_mem), (void *)&wmobj);
	ret = clSetKernelArg(sfac, 1, sizeof(cl_int), (void *)&n);
	setWorkSize(gws, lws, n/2, 1);
	ret = clEnqueueNDRangeKernel(queue, sfac, 1, NULL, gws, lws, 0, NULL, NULL);

	/* 버터플라이 연산 실행 */
	fftCore(rmobj, xmobj, wmobj, m, forward);

	/* 행렬 위치 변환 */
	ret = clSetKernelArg(trns, 0, sizeof(cl_mem), (void *)&xmobj);
	ret = clSetKernelArg(trns, 1, sizeof(cl_mem), (void *)&rmobj);
	ret = clSetKernelArg(trns, 2, sizeof(cl_int), (void *)&n);
	setWorkSize(gws, lws, n, n);
	ret = clEnqueueNDRangeKernel(queue, trns, 2, NULL, gws, lws, 0, NULL, NULL);

	/* 버터플라이 연산 실행 */
	fftCore(rmobj, xmobj, wmobj, m, forward);

	/* 하이패스 필터 적용 */
	cl_int radius = n/8;
	ret = clSetKernelArg(hpfl, 0, sizeof(cl_mem), (void *)&rmobj);
	ret = clSetKernelArg(hpfl, 1, sizeof(cl_int), (void *)&n);
	ret = clSetKernelArg(hpfl, 2, sizeof(cl_int), (void *)&radius);
	setWorkSize(gws, lws, n, n);
	ret = clEnqueueNDRangeKernel(queue, hpfl, 2, NULL, gws, lws, 0, NULL, NULL);

	/* FFT 역변환 */

	/* 버터플라이 연산 */
	fftCore(xmobj, rmobj, wmobj, m, inverse);

	/* 행렬 위치 변환 */
	ret = clSetKernelArg(trns, 0, sizeof(cl_mem), (void *)&rmobj);
	ret = clSetKernelArg(trns, 1, sizeof(cl_mem), (void *)&xmobj);
	setWorkSize(gws, lws, n, n);
	ret = clEnqueueNDRangeKernel(queue, trns, 2, NULL, gws, lws, 0, NULL, NULL);

	/* 버터플라이 연산 */
	fftCore(xmobj, rmobj, wmobj, m, inverse);

	/* 메모리 버퍼에서 결과를 읽어들이기 */
	ret = clEnqueueReadBuffer(queue, xmobj, CL_TRUE, 0, n*n*sizeof(cl_float2), xm, 0, NULL, NULL);

	/* 진폭을 계산 */
	float* ampd;
	ampd = (float*)malloc(n*n*sizeof(float));
	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++) {
			ampd[n*((i))+((j))] = (AMP(((float*)xm)[(2*n*i)+2*j], ((float*)xm)[(2*n*i)+2*j+1]));
		}
	}
	opgm.width = n;
	opgm.height = n;
	normalizeF2PGM(&opgm, ampd);
	free(ampd);

	/* 이미지 파일 출력 */
	writePGM(&opgm, "output.pgm");

	/* OpenCL 오브젝트 종료 처리 */
	ret = clFlush(queue);
	ret = clFinish(queue);
	ret = clReleaseKernel(hpfl);
	ret = clReleaseKernel(trns);
	ret = clReleaseKernel(sfac);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(xmobj);
	ret = clReleaseMemObject(rmobj);
	ret = clReleaseMemObject(wmobj);
	ret = clReleaseCommandQueue(queue);
	ret = clReleaseContext(context);

	destroyPGM(&ipgm);
	destroyPGM(&opgm);

	free(source_str);
	free(wm);
	free(rm);
	free(xm);

	return 0;
}