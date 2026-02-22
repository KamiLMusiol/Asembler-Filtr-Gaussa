#include <iostream>
//#include "Image.h"

#include "Window.h"
#include <chrono>
#include <filesystem>
#include <Windows.h>
namespace fs = std::filesystem;

#include <commdlg.h>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>
#pragma comment(lib, "Comdlg32.lib")

class controlWindow
{

	//setup dla clocka
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	

	
//koniec
	std::string pathFileRemember ="";
	
	bool overlayActive = false;

	int chosenButton;
	std::string chosenFile = "";

public:
	//setup dla clocka

	void startCountdown()
	{
		start =	std::chrono::high_resolution_clock::now();

	}
	void endCountdown()
	{
		end = std::chrono::high_resolution_clock::now();

	}

    double howLongWasOperation()
    {
        // 1. Rzutujemy na mikrosekundy (¿eby zachowaæ precyzjê)
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // 2. Dzielimy przez 1000.0, ¿eby uzyskaæ milisekundy z u³amkiem
        return duration.count() / 1000.0;
    }


    // Zmieñ pierwszy parametr z 'long long' na 'double'
    void messageBox(double ms, const std::string& blurredPath)
    {
        // U¿ywamy stringstream do ³adnego sformatowania liczby (np. 2 miejsca po przecinku)
        
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << ms; // 3 miejsca po przecinku
        std::string timeStr = stream.str();

       
        std::string msg =
            "Czas wykonania blura Gaussa: " + timeStr + " ms\n\n" // U¿ywamy sformatowanego stringa
            "Czy chcesz zapisac ZBLUROWANY obraz do pliku?";

        int result = MessageBoxA(
            nullptr,
            msg.c_str(),
            "Informacja",
            MB_YESNO | MB_ICONQUESTION
        );

        if (result != IDYES)
            return;   // user klikn¹³ "Nie" – nic nie zapisujemy

        // SprawdŸ, czy w ogóle mamy jakiœ plik do skopiowania
        if (!fs::exists(blurredPath))
        {
            MessageBoxA(nullptr,
                "Plik z zblurowanym obrazem nie istnieje.",
                "Blad",
                MB_OK | MB_ICONERROR);
            return;
        }

        //Okno "Zapisz jako" – wybór nazwy i katalogu
        char fileName[MAX_PATH] = "blur_result.jpg"; // domyœlna propozycja
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter =
            "Obrazy JPG\0*.jpg;*.jpeg\0"
            "Wszystkie pliki\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

        if (!GetSaveFileNameA(&ofn))
        {
            // user klikn¹³ "Anuluj"
            return;
        }

        try
        {
            //Kopiujemy ZBLUROWANY obraz pod now¹ nazw¹
            fs::copy_file(blurredPath,
                std::string(fileName),
                fs::copy_options::overwrite_existing);

            MessageBoxA(nullptr,
                "Zblurowany obraz zostal zapisany pomyslnie.",
                "Zapis zakonczony",
                MB_OK | MB_ICONINFORMATION);
        }
        catch (const std::exception& e)
        {
            std::string err = "Nie udalo sie zapisac pliku.\n";
            err += e.what();
            MessageBoxA(nullptr,
                err.c_str(),
                "Blad zapisu",
                MB_OK | MB_ICONERROR);
        }
		
	}

	//koniec
	std::string returnString() { return pathFileRemember; }
	bool getInfError() { return overlayActive; }

	void setButton(int a);
	int	 getButton();
	void setFile(std::string a);
	std::string getFile();
	void mainWindow(ViewClass &atp, Window &first);
	
};
