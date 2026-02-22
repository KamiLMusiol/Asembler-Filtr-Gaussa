
/*
	Projekt: Aplikacja z filtrem Gaussa – DLL ASM + DLL C++ + GUI SFML
		Opis pliku:
		Plik implementuje główną klasę Image odpowiedzialną za:
			- wczytywanie i zapis plików graficznych (biblioteka stb_image),
			- przechowywanie danych pikseli w formacie RGB,
			- obliczanie histogramów kanałów kolorów (z użyciem ASM lub C++),
			- generowanie kernela Gaussa,
			- wielowątkowe wykonywanie filtracji Gaussa (ASM lub C++),
			- kopiowanie, konstrukcję i destrukcję obiektu obrazu.

		Klasa wykorzystuje:
			- ASM_ConvolvePixel       (rozmycie piksela – wersja ASM)
			- DLL_ConvolvePixel       (rozmycie piksela – wersja C++)
			- ASM_ComputeHistogram    (histogram – ASM)
			- DLL_ComputeHistogram    (histogram – C++)
			- ASM_NormalizeKernel     (normalizacja kernela)
			- DLL_NormalizeKernel     (normalizacja kernela)
		oraz biblioteki STB do wczytywania i zapisywania obrazów.

	Algorytm działania głównych elementów:
		- gaussian_blur():wyznacza kernel Gaussa (GaussKernel),dzieli obraz na wiele wątków,każdy wątek przetwarza wybrane wiersze,używa wersji ASM lub C++ ConvolvePixel,scala wynik do jednego bufora pikseli.
		- GaussKernel():liczy filtr Gaussa jako macierz 2D,normalizuje wartości (ASM/C++),
		- returnPixels() oblicza histogram kanału RGB (ASM/C++).
		- makeColorHistogram(): generuje histogramy R, G, B  (tablkica)
		.

	Autor: Kamil Musioł
    Semestr: Semestr zimowy 2024/2025

	
*/











#define STB_IMAGE_IMPLEMENTATION // dod
#define STB_IMAGE_WRITE_IMPLEMENTATION // dod
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value) // dodane

#include "stb_image_write.h" // dod
#include "stb_image.h" // dod
#include "Image.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <conio.h>
#include <math.h>
#include <filesystem>
#include <thread>
#include "CppLibBridge.h"
#include "AsmBridge.h"
namespace fs = std::filesystem;



//konstruktor wczytujący plik
Image::Image(const char* filename)
{
	if (read(filename)){

		//std::cout << "Ok file\n";
		size = w * h * channels;
		wh = w * h;
	}

}

//konstruktor
Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels)
{
	size = w * h * channels; //  ile pamięci zajmuje
	wh = w * h;
	data = new uint8_t[size];


}

//konstruktor kopiujący
Image::Image(const Image& img) : Image(img.w, img.h, img.channels)
{
	memcpy(data, img.data, size);
	
	
}

//destruktor
Image::~Image()
{
	delete[] data;
}


//wczytanie obrazu do pamięci
bool Image::read(const char* filename)
{
	data =  stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

//zapisuje obraz 
bool Image::write(const char* filename)
{
	int sucess = stbi_write_jpg(filename, w, h, channels, data, 90);
	return sucess != 0;
}

//zapisuje obraz 
bool Image::write(std::string filename)
{
	int sucess = stbi_write_jpg(filename.c_str(), w, h, channels, data, 90);
	return sucess != 0;
}




/*
Opis:Wykonuje wielowątkową filtrację Gaussa na obrazie.  Dla każdego kanału RGB:Tworzy kernel Gaussa (GaussKernel),Dziedzili obraz na N fragmentów – każdy obsługuje inny wątek, Każdy wątek liczy piksele swojego zakresu (albo u używając funkcji asm albo c++), 
wynik trafia do bufora, po zakończeniu bufor trafia do głownego bufora
*/
Image& Image::gaussian_blur(int standard_deviation, int ker_size, bool useASM, int num_threads )
{
	
	if (num_threads < 1) num_threads = 1;

	//  Kernel Gaussa 
	std::unique_ptr<double[]> ker{ GaussKernel(standard_deviation, ker_size, useASM) };
	const double* K = ker.get();
	int ker_center = ker_size / 2;



	
	
	for (int channel = 0; channel < channels; ++channel)
	{
		std::vector<uint8_t> tmp(w * h);

		// funkcja, którą robi każdy wątek – liczy kilka wierszy
		auto worker = [&](int y_start, int y_end)
			{
				for (int position_x = y_start; position_x < y_end; ++position_x) // wiersz
				{
					for (int position_y = 0; position_y < w; ++position_y)       // kolumna
					{
					
						
						if (useASM == true)
						{
							double A = ASM_ConvolvePixel(
								data,
								w, h,
								channels, channel,
								K,
								ker_size, ker_center,
								position_x, position_y);


							tmp[position_x * w + position_y] = ASM_ByteBound((int)std::lround(A));
						}	
						else
						{
							double A = DLL_ConvolvePixel(
								data,
								w, h,
								channels, channel,
								K,
								ker_size, ker_center,
								position_x, position_y
							);

							tmp[position_x * w + position_y] = DLL_ByteBound((int)std::lround(A)); 
						}
					
					
					}
				}
			};

		//wektor wątków 
		std::vector<std::thread> threads;
		int rows_per_thread = (h + num_threads - 1) / num_threads; // ile wierszy na wątek dzielenie z zaokrągleniem do góry ostatecznie i tak nie będzie po równo

		for (int t = 0; t < num_threads; ++t)
		{
			int y_start = t * rows_per_thread;
			int y_end;
			if (y_start + rows_per_thread > h)
			{
				y_end = h;
			}
			else y_end = y_start + rows_per_thread;
			

			if (y_start < y_end)
			{
				threads.emplace_back(worker, y_start, y_end); //utwórz wątek
			}
		}

		//Czekamy na wszystkie wątki
		for (auto& th : threads)
		{
			if (th.joinable()) //czy uruchomiony
				th.join(); //zatrzymaj głowny wątek aż ten się skończy
		}

		//Przepisanie wyniku do data (dla tego kanału)
		for (int pixel = 0; pixel < w * h; ++pixel)
		{
			int idx = pixel * channels + channel;
			data[idx] = tmp[pixel];
		}
	}

	return *this;
	
}


// Gaussian filter G(x,y)=2πσ21​e−2σ2x2+y2​
/*
* funkcja generująca kernel gausssa
*/
double* Image::GaussKernel(double Standard_deviation, int size, bool useASM )
{
	double pi = 3.14159265358979323846;
	
	
	double r2, Two_sd = 2.0 * Standard_deviation * Standard_deviation;
	double* GKernel = new double[size*size];
	
	
	double sum = 0.0;
	int a = size / 2;
	
	for (int x = -a; x <= a; x++) 
	{
		for (int y = -a; y <= a; y++) 
		{
			r2 = x * x + y * y; //odległość od środka do kwadratu
			

			double value = (exp(-r2 / Two_sd)) / (pi* Two_sd);

			
			GKernel[(x + a) * size + (y + a)] = value;
			sum += value;
		}
	}

	int count = size * size;
	if (useASM == true)
	{
		
		ASM_NormalizeKernel(GKernel, count, sum);
	}
	else
	{
		
		DLL_NormalizeKernel(GKernel, count, sum);
	}


	


	return GKernel;
		
			
}


//konstruktor z pliku
Image::Image(std::string filename)
{
	if (read(filename)) {

		//std::cout << "Ok file\n";
		size = w * h * channels;
		wh = w * h;
	}
	

}


//odczytaj 
bool Image::read(std::string filename)
{
	data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
	return data != NULL;
}



//zwraca histogram pojedynczego kanalu
std::vector<int> Image::returnPixels(int channel, bool useASM)
{


	std::vector<int> table(256, 0);



	if (useASM == true)
	{
		

		ASM_ComputeHistogram(
			data,
			w * h,
			channel,
			channels,
			table.data()
		);
	}
	else
	{
	
		DLL_ComputeHistogram(
			data,          // surowy bufor
			w * h,         // liczba pikseli
			channel,       // 0/1/2
			channels,      // np. 3 (RGB)
			table.data()   // wyjściowa tablica 256 elementów
			
		);
	}


	return table;
	
	

	
	
}

//tworzy histogram calt (tablice)
void Image::makeColorHistogram(bool useASM)
{
	colorHistogram.clear();
	for (int i = 0; i < channels && i < 3; i++) // R,G,B
	{
		colorHistogram.push_back(returnPixels(i, useASM));
	}
}


