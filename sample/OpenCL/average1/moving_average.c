void moving_average(int *values, float *average, int length, int width)
{
	int i;
	int add_value;

	/* 0 ~ (width-1) 번째 요소까지의 합을 구하기 */
	add_value = 0;
	for(i = 0; i < width; i++) {
		add_value += values[i];
	}
	average[width-1] = (float)add_value;

	/* width ~ (length-1) 번째 요소까지의 합을 구하기 */
	for(i = width; i < length; i++) {
		add_value = add_value - values[i-width] + values[i];
		average[i] = (float)(add_value);
	}

	/* 0 ～ (width-2) 번째 요소를 0으로 만들기 */
	for(i = 0; i < width-1; i++) {
		average[i] = 0.0f;
	}

	/* (width-1) ~ (length-1) 번째 요소의 평균을 연산하기 */
	for(i = width-1; i < length; i++) {
		average[i] /= (float)width;
	}
}