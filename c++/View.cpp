/*
	Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
	Opis pliku: Plik implementuje klasę ViewClass — warstwę widoku/logiki
    odpowiedzialną za obsługę operacji wykonywanych na plikach graficznych

	Algorytm: 
	- imageGetting2(): sprawdza, czy plik istnieje; jeśli tak — oznacza obraz jako otwarty.
	- imageGetting3(): sprawdza plik, tworzy obiekt Image, uruchamia filtr Gaussa  (ASM lub C++), zapisuje wynik do JPG i aktualizuje histogram.



	Autor: Kamil Musioł
	Semestr: Semestr zimowy 2024/2025
	*/



#include "View.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;



void ViewClass::setButtonChosen(int a)
{
	buttonChosen = a;
}
int ViewClass::getButtonChosen()
{
	return buttonChosen;
}




//funkcja odpowiedzialna zawczytanie obrazu bez flitracji - filename nazwa obrazu do wczytania i pokazania
void ViewClass::imageGetting2( std::string fileName)
{
	goodPhoto = this->openingPhoto(fileName);
		if(goodPhoto == true)
	openedPhoto = true;

}



/*
	
	Opis:
		Wykonuje pełny proces filtracji Gaussa:
			1) Sprawdzenie istnienia pliku wejściowego,
			2) Utworzenie obiektu Image,
			3) Wykonanie filtracji Gaussa (gaussian_blur) w wersji ASM lub C++,
			4) Zapis wyniku do pliku JPG ("test3.jpg"),
			5) Obliczenie histogramu z obrazu wynikowego,
			6) Zwrócenie nazwy pliku wynikowego.

	Parametry:
		fileName      – nazwa pliku wejściowego 
		odchylenie    – sigma filtru Gaussa,
		size          – rozmiar kernela,
		useASM        – true → użyj implementacji ASM, false → C++,
		liczba_watkow – liczba wątków roboczych do filtracji.

	
		std::string – nazwa pliku wynikowego ("test3.jpg"). zwracany
*/
std::string ViewClass::imageGetting3(std::string fileName, int odchylenie, int size, bool useASM, int liczba_watkow)
{
	std::string photoName = "";
	goodPhoto =  this->openingPhoto(fileName);
	
	if (goodPhoto == true)
	{
		openedPhoto = true;
		photoName = "test3.jpg";
		Image test(fileName);

		

		histImage = std::make_unique<Image>(fileName); //  lokalnego Image na pole
		histImage->gaussian_blur(odchylenie, size,  useASM, liczba_watkow);
		histImage->write(photoName);
		histImage->makeColorHistogram(useASM);                // policz histogram tego co rysujemy
		

	}

	return photoName;
	
	
}


//sprawdza czy plik istnieje
bool ViewClass::openingPhoto(std::string nampePhoto)
{

	fs::path current = fs::current_path();
	fs::path file = current / nampePhoto;

	if (fs::exists(file)) {
		std::string cmd = "start \"\" \"" + file.string() + "\"";
		//system(cmd.c_str());
		//std::cout << "WOW\m";
		return true;
	}
	else {
		//std::cout << "Brak pliku: " << file << "\n";
		return false;
	}
}



