#include <iostream>
#include <immintrin.h>
#include <omp.h>
#include "matoperation.hpp"
//实现头函数的

//size_t 整型 unsigned int 或者 unsigned long
// 感觉就是直接算？
float dotproduct1(const float *p1, const float * p2, size_t n)
{
    float sum = 0.0f;
    //for (int i = n-1; i>=0; i--):内存从后向前发现差别不大
    // for (size_t i = n-1; i>=0; i--) 会一直循环下去，因为size_t是恒大于零的
    for (size_t i = 0; i < n; i++)
        sum += (p1[i] * p2[i]);
    return sum;
}

float dotproduct2(const float *p1, const float * p2, size_t n)
{
    if(n % 8 != 0)
    {
        std::cerr << "The size n must be a multiple of 8." <<std::endl;
        return 0.0f;
    }

    float sum = 0.0f;
    
    /////减少判断的时间，如果是定长，可以尝试拷贝多次？这个版本会快一点
    ////速度和调用的顺序有关系-》如果2在1前面，2就会慢一点，1在2前面1就会慢一点，可能是因为读数据变快了（memory的io）
    ///再前面运行了几次之后，然后再比较会发现2比1要快一点点
    for (size_t i = 0; i < n; i+=8)
    {
        sum += (p1[i] * p2[i]);
        sum += (p1[i+1] * p2[i+1]);
        sum += (p1[i+2] * p2[i+2]);
        sum += (p1[i+3] * p2[i+3]);
        sum += (p1[i+4] * p2[i+4]);
        sum += (p1[i+5] * p2[i+5]);
        sum += (p1[i+6] * p2[i+6]);
        sum += (p1[i+7] * p2[i+7]);
    }
    return sum;

}

float dotproduct3(const float *p1, const float * p2, size_t n)
{
    if(n % 8 != 0)
    {
        std::cerr << "The size n must be a multiple of 8." <<std::endl;
        return 0.0f;
    }
    //std:: cout << "in function" <<std:: endl;

    float sum[8] = {0};
    ///寄存器，理论上加速8倍，实际上加速3倍
    __m256 a, b;
    __m256 c = _mm256_setzero_ps();
    std::cout << "before for" << std::endl;
    //把数据从内存放到寄存器里面
    for (size_t i = 0; i < n; i+=8)
    {
        //std::cout << "in for" << std::endl;
        a = _mm256_load_ps(p1 + i); 
        b = _mm256_load_ps(p2 + i);
        c =  _mm256_add_ps(c, _mm256_mul_ps(a, b)); 
        // 乘法 s：float，寄存器操作一定要调用指令
    }
    std::cout << "after for" << std::endl;

    _mm256_store_ps(sum, c); // load的反操作
    return (sum[0]+sum[1]+sum[2]+sum[3]+sum[4]+sum[5]+sum[6]+sum[7]);
}

///并行里面可能会出现数据冲突，对于一个sum不能
float dotproduct4(const float *p1, const float * p2, size_t n)
{
    if(n % 8 != 0)
    {
        std::cerr << "The size n must be a multiple of 8." <<std::endl;
        return 0.0f;
    }

    float sum[8] = {0};
    __m256 a, b;
    __m256 c = _mm256_setzero_ps();

    #pragma omp parallel for // 用计算机的八个内核一起使用
    for (size_t i = 0; i < n; i+=8)
    {
        a = _mm256_load_ps(p1 + i);
        b = _mm256_load_ps(p2 + i);
        c =  _mm256_add_ps(c, _mm256_mul_ps(a, b));
    }
    _mm256_store_ps(sum, c);
    return (sum[0]+sum[1]+sum[2]+sum[3]+sum[4]+sum[5]+sum[6]+sum[7]);
}
