#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel void compute(__global char* string, __global int* condVar, __global int* RLOG)
{
    int id = get_global_id(0);
    if(condVar[id]){
      string[id] = 'C';
      RLOG[id] += 1;
    }
    string[9] = '\0';
    atomic_inc(&RLOG[condVar[id]]);
}

__kernel void conflict_detect(__global int* index, __global int* count)
{
}
