#pragma once
#include <cstdint>
#include <Windows.h>

using ComputeHistogramAsmFn = void(*)(const uint8_t*, int, int, int, int*);

bool LoadAsmDll();

using ComputeHistogramAsmFn = void(*)(const uint8_t*, int, int, int, int*);


extern ComputeHistogramAsmFn ASM_ComputeHistogram;


using AddTwoIntsAsmFn = long long(*)(long long, long long);

extern AddTwoIntsAsmFn ASM_AddTwoInts;

using ByteBoundAsmFn = uint8_t(*)(int);
extern ByteBoundAsmFn ASM_ByteBound;


using NormalizeKernelAsmFn = void(*)(double*, int, double);
extern NormalizeKernelAsmFn ASM_NormalizeKernel;

using ConvolvePixelAsmFn = double(*)(const uint8_t*, int, int, int, int,
    const double*, int, int,
    int, int);

extern ConvolvePixelAsmFn ASM_ConvolvePixel;



