
#include <iostream>
#include "Image.h"
#include <memory>

class ViewClass
{
	bool useASM;
	int buttonChosen;
	bool openedPhoto = false;
	bool goodPhoto = true;


	std::unique_ptr<Image> histImage; 

public:
	Image* getHistogramImage() { return histImage.get(); }
	bool isOpened() { return openedPhoto; }
	void setClosed() { openedPhoto = false; }
	bool getResultPhoto() { return goodPhoto; }
	void setButtonChosen(int numberButton);
	int getButtonChosen();


	ViewClass() {};


	void mainProgram();
	void mainProgram2();

	void imageGetting2(std::string fileName);
	std::string imageGetting3(std::string fileName , int odyclenie, int size, bool useASM, int liczba_watkow = 1);

	bool openingPhoto(std::string nampePhoto);

	void listJpgFiles();

	void setAsm(bool a) {
		useASM = a;
	}

	



};
