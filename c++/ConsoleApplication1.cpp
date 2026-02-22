/*
    Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
    Opis: Plik zawiera funkcję główną programu, inicjuje biblioteki dll C++ i asm, tworzy i uruchamia obiekt interfejsu użytkownika
    wersja 1
    Autor: Kamil Musioł
    Semestr: Semestr zimowy 2024/2025

*/



#include "controller.h"

#include "CppLibBridge.h"    
#include "AsmBridge.h"



int main()
{
   
    LoadCppDll();
    LoadAsmDll();

    

    ViewClass atp;
    Window first;
    controlWindow flow;

    while (first.getWindowIsOpen())
    {
        first.update();

        int act = first.getButtonAction();
        flow.setButton(act);
        flow.mainWindow(atp, first);

        // RENDER – parametr jest ignorowany w środku, więc podajemy dummy
        first.render();
    }
    
   
    
    
   

}


