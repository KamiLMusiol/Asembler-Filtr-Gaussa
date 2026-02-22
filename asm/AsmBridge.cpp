/*
    Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
    Opis pliku: Plik zawiera implementacjê modu³u mostu (bridge) odpowiedzialnego za dynamiczne ³adowanie biblioteki MyAsmLib.dll oraz pobieranie wskaŸników do funkcji napisanych w asemblerze 
    dynamiczne ³¹czenie g³ownego programu z bibliotek¹ ASM, weryfikacje czy funkcje dzia³aj¹ je¿eli nie wysy³aj¹ message boxa
    Autor: Kamil Musio³
    Semestr: Semestr zimowy 2024/2025
    */
#include "AsmBridge.h"
#include <iostream>

static HMODULE g_hAsmDll = nullptr;
ComputeHistogramAsmFn ASM_ComputeHistogram = nullptr;
AddTwoIntsAsmFn ASM_AddTwoInts = nullptr;
ByteBoundAsmFn ASM_ByteBound = nullptr;

NormalizeKernelAsmFn ASM_NormalizeKernel = nullptr;

ConvolvePixelAsmFn ASM_ConvolvePixel = nullptr;


/*
   
        £aduje dynamicznie bibliotekê MyAsmLib.dll i pobiera adresy funkcji
        napisanych w asemblerze. U¿ywamy jej po tem po to by u¿yæ funkcji w g³ównej aplikacji

   
        zwraca bool – true jeœli DLL za³adowano poprawnie, false jeœli wyst¹pi³ b³¹d (DLL lub funkcji brak).

        Zmienne globalne (tworzone w tym module):
        g_hAsmDll                 – uchwyt do za³adowanej biblioteki
        ASM_ComputeHistogram      – wskaŸnik do funkcji histogramu
        ASM_AddTwoInts            – funkcja dodawania w ASM
        ASM_ByteBound             – funkcja saturacji bajtu
        ASM_NormalizeKernel       – funkcja normalizacji kernela
        ASM_ConvolvePixel         – funkcja obliczania rozmycia jednego piksela

        jeœli DLL ju¿ za³adowano, zwraca true.
        W przypadku b³êdu wyœwietlany jest MessageBox z informacj¹.
*/


bool LoadAsmDll()
{
    if (g_hAsmDll) return true;



    g_hAsmDll = LoadLibraryA("MyAsmLib.dll");
    if (!g_hAsmDll)
    {
        MessageBoxA(nullptr, "Nie mogê za³adowaæ MyAsmLib.dll",
            "B³¹d", MB_OK | MB_ICONERROR);
        return false;
    }

   

    ASM_AddTwoInts = (AddTwoIntsAsmFn)GetProcAddress(g_hAsmDll, "AddTwoIntsAsm");

    if (!ASM_AddTwoInts)
    {
        MessageBoxA(nullptr, "Brak funkcji AddTwoIntsAsm", "B³¹d", MB_OK | MB_ICONERROR);
        return false;
    }



    ASM_ByteBound =
        (ByteBoundAsmFn)GetProcAddress(g_hAsmDll, "ByteBoundAsm");

    if (!ASM_ByteBound)
    {
        MessageBoxA(nullptr, "Brak ByteBoundAsm w DLL", "B³¹d", MB_OK);
        return false;
    }

    ASM_ComputeHistogram =
        (ComputeHistogramAsmFn)GetProcAddress(g_hAsmDll, "ComputeHistogramAsm");

    if (!ASM_ComputeHistogram)
    {
        MessageBoxA(nullptr, "Brak ComputeHistogramAsm w DLL",
            "B³¹d", MB_OK | MB_ICONERROR);
        return false;
    }


    ASM_NormalizeKernel =
        (NormalizeKernelAsmFn)GetProcAddress(g_hAsmDll, "NormalizeKernelAsm");

    if (!ASM_NormalizeKernel)
    {
        MessageBoxA(nullptr, "Brak NormalizeKernelAsm w MyAsmLib.dll",
            "B³¹d", MB_OK | MB_ICONERROR);
        return false;
    }
    ASM_ConvolvePixel =
        (ConvolvePixelAsmFn)GetProcAddress(g_hAsmDll, "ConvolvePixelAsm");

    if (!ASM_ConvolvePixel)
    {
        MessageBoxA(nullptr, "Brak ConvolvePixelAsm w MyAsmLib.dll",
            "B³¹d", MB_OK | MB_ICONERROR);
        return false;
    }


   



    return true;
}
