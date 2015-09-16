__kernel void moving_average_vec4(__global int4 *values, __global float4 *average, int length, int width)
{
	int i;
	int4 add_value; /* 4개 종목 데이터 값의 합산 결과를 저장하기 위한 변수 */

	/* 0 ~ (width-1) 번째 요소까지의 합을 구하기(4개 종목 데이터 값을 동시에 처리) */
	add_value = (int4)0;
	for(i = 0; i < width; i++) {
		add_value += values[i];
	}
	average[width-1] = convert_float4(add_value);

	/* width ~ (length-1) 번째 요소까지의 합을 구하기(4개 종목 데이터 값을 동시에 처리) */
	for(i = width; i < length; i++) {
		add_value = add_value - values[i-width] + values[i];
	average[i] = convert_float4(add_value);
	}

	/* 0 ～ (width-2) 번째 요소를 0으로 지우기(4개 종목 데이터 값을 동시에 처리) */
	for(i = 0; i < width-1; i++) {
		average[i] = (float4)(0.0f);
	}

	/* (width-1) ~ (length-1) 번째 요소의 평균 연산(4개 종목 데이터 값을 동시에 처리) */
	for(i = width-1; i < length; i++) {
		average[i] /= (float4)width;
	}
}