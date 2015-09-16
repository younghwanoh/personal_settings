#include <stdio.h>
#include <stdlib.h>

/* 주가 데이터를 읽어들이기 */
int stock_array1[] = {
	#include "stock_array1.txt"
};

/* 이동평균 폭 설정 */
#define WINDOW_SIZE (13)

int main(int argc, char *argv[])
{
	float *result;

	int data_num = sizeof(stock_array1) / sizeof(stock_array1[0]);
	int window_num = (int)WINDOW_SIZE;

	int i;

	/* 처리 결과를 저장할 메모리를 할당 */
	result = (float *)malloc(data_num*sizeof(float));

	/* 이동평균함수 호출 */
	moving_average(stock_array1, result, data_num, window_num);

	/* 결과 출력 */
	for(i = 0; i < data_num; i++) {
		printf("result[%d] = %f\n", i, result[i]);
	}

	/* 할당한 메모리를 해제 */
	free(result);
}