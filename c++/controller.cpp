
/*
	Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
	Opis pliku: Plik zawiera implementację metod klasy controlWindow, która pełni rolę kontrolera aplikacji (wzorzec MVC). odczyt działania wybranego przez użytkownika (przycisk), przekazanie parametrów z GUI do logiki przetwarzania obrazu, uruchomienie filtru Gaussa (ASM lub C++) z odpowiednimi ustawieniami, 
	pomiar czasu wykonania operacji, przygotowanie danych do wizualizacji (obraz przed/po, histogram)

	Algorytm: Metoda mainWindow - odczytuje nazwę pliku oraz liczbę wątków z okna, na podstawie wybranego przycisku wykonuje jedną z akcjj: wczytuje i wyświetla wybrany obraz, wykonuje gaussa obrazu (funkcje) i go wyświetla w oknie - i kilka innych rzeczy, hisotgram, opcja zapisu
	pozostałe metody służą do ustawiania i pobierania wybranego przyciku/pliku 


	Autor: Kamil Musioł
	Semestr: Semestr zimowy 2024/2025
	*/


#include "controller.h"


	/*
		Methoda: mainWindow
		opis:	Główna metoda kontrolera obsługująca logikę programu na podstawie aktualnie wybranego przycisku w interfejsie użytkownika.

		parametry:
			atp   – referencja do obiektu ViewClass odpowiedzialnego za
					operacje na obrazach (wczytywanie, filtr Gaussa, histogram).
			first – referencja do obiektu Window reprezentującego główne okno GUI.
					Z tego obiektu pobierane są parametry (nazwa pliku, kernel,
					odchylenie, liczba wątków) oraz ustawiane są efekty (obrazy
					przed/po, komunikaty błędów, histogram).

		Działanie (switch case):
			case 2 (button open file):
				- pobiera nazwę pliku,
				- próbuje wczytać obraz i ustawić go jako „przed filtrowaniem”.
			case 3 (button open file):
				- sprawdza, czy pola liczbowe (kernel, sigma) nie są puste,
				- waliduje wartości kernela i sigmy (parzystość, zakres),
				- ustawia, czy używać implementacji ASM czy C++,
				- uruchamia stoper,
				- wywołuje filtr Gaussa z uwzględnieniem liczby wątków,
				- zatrzymuje stoper i wyświetla czas w oknie dialogowym,
				- wczytuje i wyświetla obraz po filtracji,
				- ustawia obraz źródłowy dla histogramu (ilustratingAfter).

		Returns:
			Brak wartości zwrotnej (void). Efekty działania widoczne są w GUI
			oraz w polu czasu operacji.
	*/


void controlWindow::mainWindow(ViewClass& atp, Window& first)
{
	
	int threads = first.getThreadsFromCombo();
	int choosingType = chosenButton;	 //chosen button z maina
	setFile(first.getTextValue());
	
	auto h = first.getWindowSize().y;
	

	switch (chosenButton)
	{

	case 2:  
		
		
		atp.imageGetting2(getFile());
		first.initPhotoBefore(getFile());
		
		
		break;

		
		
		
		break;
	case 3: 

		if (first.checkEmptyNumber())
		{
			first.setToErrorNumber(true);
			break;
		}

		if (first.getTextValue() == "")
			break;
		
		if (!first.readAndValidateKernelSigma(first.getSize(), first.getOdchylenie()))
		{
			first.setToErrorNumber(true);
			break;
		}
		first.setToErrorNumber(false);
		atp.setAsm(first.getUseAsm());
		pathFileRemember = fs::current_path().string();
		
		atp.setAsm(first.getUseAsm());
		startCountdown();
		pathFileRemember = pathFileRemember + "\\" + atp.imageGetting3(getFile(), first.getOdchylenie() , first.getSize(), first.getUseAsm(), threads);
		endCountdown();
		std::cout << "Czas: " << howLongWasOperation() << std::endl;
		this->messageBox(howLongWasOperation(), pathFileRemember);

		first.initPhotoAfter(pathFileRemember);

		
		first.getHistAfter().setImage(atp.getHistogramImage());
		

		
		break;
		
		

		
	default:;
	}
}


//ustawia wybrany przycisk zwykly set
void controlWindow::setButton(int a)
{
	chosenButton = a;
}

//zwraca button zwykly get
int	 controlWindow::getButton()
{
	return chosenButton;
}


//set dla pliku
void controlWindow::setFile(std::string a)
{
	chosenFile = a;
}

//returnuje get
std::string	 controlWindow::getFile()
{
	return chosenFile;
}