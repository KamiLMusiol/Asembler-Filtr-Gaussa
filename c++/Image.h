#include <iostream>
#include <cstdint>   // std::uint8_t
#include <cstddef>  
#include <vector>
struct Image
{
	
	//std::vector< uint8_t> data;
	uint8_t* data = NULL; // leci co 3 r + g + b
	size_t size = 0;
	int w;
	int h;
	int channels; //(r + g + b)
	int wh;

	std::vector<std::vector<int>> colorHistogram;  // 3 wektory po 256 wartoœci

	Image(const char* filename);
	Image(int w, int h, int channels);
	Image(const Image& img);
	~Image();

	bool read(const char* filename);
	bool write(const char* filename);
	void openingPhoto(std::string name);

	

	Image& gaussian_blur(int Standard_deviation, int size, bool useASM, int num_threads = 1);

	 uint8_t byte_bound(int value) {
		if (value < 0) return 0;
		if (value > 255) return 255;
		return (value);
	}

	

	 double* GaussKernel(double Standard_deviation, int size, bool useASM);

	 void menu();

	 void imageGetting(int a);


	 Image(std::string filename);
	 bool read(std::string filename);
	 bool write(std::string filename);


	 std::vector<int> returnPixels(int RGB, bool useASM);
	 void makeColorHistogram(bool useASM);


	


};