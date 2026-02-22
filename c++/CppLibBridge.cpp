/*
    Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
    Opis: Plik implementuje modu³ bridge odpowiedzialny za dynamiczne ³adowanie funkcji znajduj¹cych siê w bibliotece MyCppLib.dll.Poœrednicotwo g³owna a dll c++
    Funkcje:
    - ComputeHistogram      – obliczanie histogramu kana³u RGB
    - ByteBoundCpp          – saturacja (0–255)
    - NormalizeKernelCpp    – normalizacja kernela Gaussa
    - ConvolvePixelCpp      – rozmycie pojedynczego piksela 

    w przypadku b³êduw message box

    dzia³anie: pobierna funkcje lepiej opisane ni¿ej

    Autor: Kamil Musio³
    Semestr: Semestr zimowy 2024/2025

*/











#include "CppLibBridge.h"
#include <windows.h>
#include <stdexcept>
#include <iostream>

HMODULE g_cppDll = nullptr;

// wskaŸnik do funkcji w DLL

void (*DLL_ComputeHistogram)(const uint8_t*, int, int, int, int*) = nullptr;
ByteBoundCppFn DLL_ByteBound = nullptr;

NormalizeKernelCppFn DLL_NormalizeKernel = nullptr;
ConvolvePixelCppFn DLL_ConvolvePixel = nullptr;   // NOWE


/*
    Fun: LoadCppDll
    Description:  Dynamicznie ³aduje bibliotekê MyCppLib.dll oraz pobiera wskaŸniki  do wszystkich funkcji eksportowanych przez tê bibliotekê. Funkcja pe³ni rolê poœrednika pomiêdzy aplikacj¹ a implementacj¹ algorytmów w bibliotece C++

    Returns:
        void
        (ale rzuca std::runtime_error, jeœli wyst¹pi powa¿ny b³¹d podczas
        ³adowania DLL lub pobierania funkcji)

    Global variables modified:
        g_cppDll              – uchwyt do DLL
        DLL_ComputeHistogram  – wskaŸnik do funkcji histogramu
        DLL_ByteBound         – wskaŸnik saturacji (0–255)
        DLL_NormalizeKernel   – wskaŸnik normalizacji kernela
        DLL_ConvolvePixel     – wskaŸnik filtru Gaussa dla 1 piksela

    Uwagi:
        - Funkcja jest odporna na wielokrotne wywo³anie (jeœli DLL ju¿ za³adowany ? return).
        - Jeœli nie mo¿na za³adowaæ biblioteki, zg³aszany jest wyj¹tek.
        - Niektóre b³êdy s¹ sygnalizowane poprzez okna MessageBoxA.
*/

void LoadCppDll()
{
    if (g_cppDll) return; // ¿eby nie ³adowaæ drugi raz

    g_cppDll = LoadLibraryA("MyCppLib.dll");
    if (!g_cppDll)
        throw std::runtime_error("Nie mogê za³adowaæ MyCppLib.dll");

    DLL_ComputeHistogram =
        (void(*)(const uint8_t*, int, int, int, int*))
        GetProcAddress(g_cppDll, "ComputeHistogram");

    if (!DLL_ComputeHistogram)
        throw std::runtime_error("Brak ComputeHistogram w DLL!");

    DLL_ByteBound =
        (ByteBoundCppFn)GetProcAddress(g_cppDll, "ByteBoundCpp");

    if (!DLL_ByteBound)
        throw std::runtime_error("Brak ByteBoundCpp w MyCppLib.dll");


    DLL_NormalizeKernel =
        (NormalizeKernelCppFn)GetProcAddress(g_cppDll, "NormalizeKernelCpp");

    if (!DLL_NormalizeKernel)
    {
        MessageBoxA(nullptr, "Brak NormalizeKernelCpp w MyCppLib.dll",
            "B³¹d", MB_OK | MB_ICONERROR);
        return; // <-- MUSI tu byæ return
    }

    DLL_ConvolvePixel =
        (ConvolvePixelCppFn)GetProcAddress(g_cppDll, "ConvolvePixelCpp");

    if (!DLL_ConvolvePixel)
    {
        MessageBoxA(nullptr, "Brak ConvolvePixelCpp w MyCppLib.dll",
            "B³¹d", MB_OK | MB_ICONERROR);
        return;
    }

}

// Zwalnia bibliotekê MyCppLib.dll z pamiêci i zeruje uchwyt.
void UnloadCppDll()
{
    if (g_cppDll)
    {
        FreeLibrary(g_cppDll);
        g_cppDll = nullptr;
    }
}
