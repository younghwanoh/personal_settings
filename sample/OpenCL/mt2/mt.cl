typedef struct mt_struct_s {
	uint aaa;
	int mm, nn, rr, ww;
	uint wmask, umask, lmask;
	int shift0, shift1, shiftB, shiftC;
	uint maskB, maskC;
} mt_struct;

/* seed를 이용해 상태를 초기화 */
void sgenrand_mt(uint seed, __local const mt_struct *mts, __local uint *state) {
	int i;
	for(i = 0; i < mts -> nn; i++) {
	    state[i] = seed;
	    seed = (1812433253 * (seed ^ (seed >> 30))) + i + 1;
	}
	for(i = 0; i < mts -> nn; i++)
	    state[i] &= mts -> wmask;
}

/* 상태를 갱신 */
void update_state(__local const mt_struct *mts, __local uint *st, int wlid) {
	int n = 17, m = 8;
	uint aa = mts -> aaa, x;
	uint uuu = mts -> umask, lll = mts -> lmask;
	int k, lim;

	if(wlid < 9) {
	   /* 보통 인덱스 k+1와 k+m 에 접근할 때는 의존성이 발생하나, 워프 내부는 동기 처리됨 */
	   /* 따라서 st[k]로의 쓰기 연산이 st[k+1]이나 st[k+m]의 읽기 연산보다 */
	   /* 나중에 일어난다면 문제 되지 않음 */

	   k = wlid;
	   x = (st[k]&uuu) | (st[k+1]&lll);
	   st[k] = st[k+m] ^ (x >> 1) ^ (x&1U ? aa : 0U);
	}
	if(wlid < 7) {
	   /* 위처럼 보통은 인덱스 k+m-n에 의존성이 발생하나, 워프 내부는 동기 처리됨 */
	   /* 이 시점에서는 쓰기가 완료되었으므로 문제 되지 않음 */

	   k = wlid + 9;
	   x = (st[k]&uuu) | (st[k+1]&lll);
	   st[k] = st[k+m-n] ^ (x >> 1) ^ (x&1U ? aa : 0U);
	}
	if(wlid == 0) {
	   x = (st[n-1]&uuu) | (st[0]&lll);
	   st[n-1] = st[m-1] ^ (x >> 1) ^ (x&1U ? aa : 0U);
	}
}

inline void gen(__global uint *out, const __local mt_struct *mts, __local uint *state, int num_rand, int wlid) {
	int i, j, n, nn = mts -> nn;
	n = (num_rand+(nn-1)) / nn;

	for(i = 0; i < n; i++) {
	    int m = nn;
	    if(i == n-1) m = num_rand%nn;

	    update_state(mts, state, wlid);

	    if(wlid < m) { /* 하나의 워프에 포함된 m개의 워크 아이템으로 tempering */
	       int j = wlid;
	       uint x = state[j];
	       x ^= x >> mts -> shift0;
	       x ^= (x << mts -> shiftB) & mts -> maskB;
	       x ^= (x << mts -> shiftC) & mts -> maskC;
	       x ^= x >> mts -> shift1;
	       out[i*nn + j] = x;
	    }
	}
}

__kernel void genrand(__global uint *out,__global mt_struct *mts_g,int num_rand, int num_generator, uint num_param_per_warp, __local uint *state_mem, __local mt_struct *mts) {
	int warp_per_compute_unit = 4;
	int workitem_per_warp = 32;
	int wid = get_group_id(0);
	int lid = get_local_id(0);
	int warp_id = wid * warp_per_compute_unit + lid / workitem_per_warp;
	int generator_id, end;
	int wlid = lid % workitem_per_warp; /* 워프 안의 로컬 ID */

	__local uint *state = state_mem + warp_id*17; /* 상태를 로컬 메모리에 저장 */

	end = num_param_per_warp*warp_id + num_param_per_warp;

	if(end > num_generator)
	   end = num_generator;

	mts = mts + warp_id;


	/* 워크 그룹의 각 생성 파라미터로 루프 실행 */
	for(generator_id = num_param_per_warp*warp_id; generator_id < end; generator_id++) {
	    if(wlid == 0) {
	       /* 생성 파라미터를 로컬 메모리에 복사 */
	       mts -> aaa = mts_g[generator_id].aaa;
	       mts -> mm = mts_g[generator_id].mm;
	       mts -> nn = mts_g[generator_id].nn;
	       mts -> rr = mts_g[generator_id].rr;
	       mts -> ww = mts_g[generator_id].ww;
	       mts -> wmask = mts_g[generator_id].wmask;
	       mts -> umask = mts_g[generator_id].umask;
	       mts -> lmask = mts_g[generator_id].lmask;
	       mts -> shift0 = mts_g[generator_id].shift0;
	       mts -> shift1 = mts_g[generator_id].shift1;
	       mts -> shiftB = mts_g[generator_id].shiftB;
	       mts -> shiftC = mts_g[generator_id].shiftC;
	       mts -> maskB = mts_g[generator_id].maskB;
	       mts -> maskC = mts_g[generator_id].maskC;
	       sgenrand_mt(0x33ff*generator_id, mts, (__local uint*)state); /* 난수 초기화 */
	    }
	    gen(out + generator_id*num_rand, mts, (__local uint*)state, num_rand, wlid); /* 난수 생성 */
	}
}

/* 원 안에 포함되는 점 수를 세기 */
__kernel void calc_pi(__global uint *out, __global uint *rand, int num_rand_per_compute_unit, int num_compute_unit, int num_rand_all, __local uint *count_per_wi) {
	int gid = get_group_id(0);
	int lid = get_local_id(0);

	int count = 0;
	int i, end, begin;

	/* 워크 그룹이 처리를 담당할 인덱스의 범위 */
	begin = gid * num_rand_per_compute_unit;
	end = begin + num_rand_per_compute_unit;

	/* 마지막 부분이 생성된 난수보다 많을 때는 처리 수를 줄임 */
	if(end > num_rand_all)
	   end = num_rand_all;

	rand += lid;

	/* 한 번에 128 요소 단위로 처리 */
	for(i = begin; i < end-128; i += 128) {
	    float x, y, len;
	    x = ((float)(rand[i] >> 16))/65535.0f; /* x 좌표 */
	    y = ((float)(rand[i]&0xffff))/65535.0f; /* y 좌표 */
	    len = (x*x + y*y); /* 원점에서의 거리 */

	    if(len < 1) { /* sqrt(len) < 1 = len < 1 */
	       count++;
	    }
	}

	/* 남은 요소를 처리 */
	if((i + lid) < end) {
	   float x, y, len;
	   x = ((float)(rand[i] >> 16))/65535.0f; /* x 좌표 */
	   y = ((float)(rand[i]&0xffff))/65535.0f; /* y 좌표 */
	   len = (x*x + y*y); /* 원점에서의 거리 */

	   if(len < 1) { /* sqrt(len) < 1 = len < 1 */
	      count++;
	   }
	}

	count_per_wi[lid] = count;

	/* 각 워크 아이템의 카운터 값을 더하기 */

	barrier(CLK_LOCAL_MEM_FENCE); /* 모든 워크 아이템의 처리가 완료될 때까지 대기 */

	if(lid == 0) {
	   int count = 0;
	   for(i = 0; i < 128; i++) {
	       count += count_per_wi[i]; /* 카운터의 합계를 구하기 */
	   }
	   out[gid] = count;
	}
}