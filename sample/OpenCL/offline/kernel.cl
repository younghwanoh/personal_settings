//#pragma OPENCL EXTENSION cl_khr_spir : enable

__kernel void vecAdd(__global float* a)
{
    int gid = get_global_id(0);

    a[gid] += a[gid];
}
