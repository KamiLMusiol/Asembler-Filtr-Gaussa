#pragma once
#include <cstdint>

void LoadCppDll();
void UnloadCppDll();


extern void (*DLL_ComputeHistogram)(const uint8_t*, int, int, int, int*);


using ByteBoundCppFn = uint8_t(*)(int);

extern ByteBoundCppFn DLL_ByteBound;


using NormalizeKernelCppFn = void(*)(double*, int, double);
extern NormalizeKernelCppFn DLL_NormalizeKernel;

// --------------------------------------
// ??? ConvolvePixelCpp – HOT LOOP dla Gaussa ???
// --------------------------------------
using ConvolvePixelCppFn = double(*)(const uint8_t*, int, int, int, int,
    const double*, int, int,
    int, int);

extern ConvolvePixelCppFn DLL_ConvolvePixel;