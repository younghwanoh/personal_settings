__kernel void moving_average_many(__global int4 *values, __global float4 *average, int length, int name_num, int width)
{
	int i, j;
	int loop_num = name_num / 4; /* 4개 종목의 동시 처리를 몇 회 반복할 지 계산 */
	int4 add_value;

	for(j = 0; j < loop_num; j++) {
	/* 0 ~ (width-1) 번째 요소까지의 합을 구하기(4개 종목 데이터 값을 동시에 처리) */
		add_value = (int4)0;
		for(i = 0; i < width; i++) {
			add_value += values[i*loop_num+j];
		}
		average[(width-1)*loop_num+j] = convert_float4(add_value);

		/* width ~ (length-1) 번째 요소까지의 합을 구하기(4개 종목 데이터 값을 동시에 처리) */
		for(i = width; i < length; i++) {
			add_value = add_value - values[(i-width)*loop_num+j] + values[i*loop_num+j];
			average[i*loop_num+j] = convert_float4(add_value);
		}

		/* 0 ～ (width-2) 번째 요소를 지우기(4개 종목 데이터 값을 동시에 처리) */
		for(i = 0; i < width-1; i++) {
			average[i*loop_num+j] = (float4)(0.0f);
		}

		/* (width-1) ~ (length-1) 번째 요소의 평균 계산(4개 종목 데이터 값을 동시에 처리) */
		for(i = width-1; i < length; i++) {
			average[i*loop_num+j] /= (float4)width;
		}
	}
}