#pragma once


#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <Windows.h>
#include "View.h"
#include <filesystem>

namespace fs = std::filesystem;


struct SpriteWithTexture {
	sf::Texture texture;
	sf::Sprite  sprite;
	bool canBeShown = false;


	void clearTexture()
	{
		sprite = sf::Sprite();   // usuwa powi¹zanie sprite'a z tekstur¹
		texture = sf::Texture(); // resetuje teksturê (zwalnia pamiêæ GPU)
		canBeShown = false;
	}

	

};



class Window;

class MainButton
{
	sf::RectangleShape button;
	sf::Text buttonText;
	sf::FloatRect textBounds; // ile miejsca zajmuje
	
	Window* window = nullptr;

	// stany
	bool hovered = false;
	bool pressed = false;
	

public:


	// konstruktory
	MainButton() = default;
	MainButton(const sf::RectangleShape& button, const sf::Text& buttonText, Window& w) : button(button), buttonText(buttonText), window(&w) {};

	//tworzenie 
	void createButton(const sf::Vector2f& size, float x, float y, const sf::Font& font, std::string text, int charakterSize,const sf::Color& fillColor);
	void setWindow(Window* w) { window = w; }

	void drawButton();

	sf::FloatRect getGlobalBounds();
	
};


class MainTextField
{

	sf::Clock caretClk;
	sf::RectangleShape textBox;
	sf::Text           textLabel;   // to co widaæ w œrodku
	bool               textFocused = false;
	std::string        textValue;   // wpisany tekst
	sf::Text textDescription; // napis nad polem tekstowym

	Window* window = nullptr;

public:

	
	MainTextField() {};

	//funkcje do inicjacji
	void setWindow(Window* w) { window = w; }
	void createTextField(float x, float y, float width, std::string tekst, sf::Font& font);


	//funkcje do ci¹g³ego wyswietlania
	void renderText(float outlineFickness);
	void drawText();
	void setFocus(bool a) {
		if (textFocused != a) {
			textFocused = a;
			if (textFocused) caretClk.restart(); // start migania od „widoczny”
		}
	}
	sf::Clock& getClock() { return caretClk; }


	//funkcje zwracaj¹ce wartosci potrzbene w funckajch od window - do clocka dokladniej
	bool& getFocus() { return textFocused; }
	std::string& getValue() { return textValue; }
	sf::Text& getLabel() { return textLabel; }
	sf::RectangleShape& getBox(){ return textBox; }

};



class Histogram
{
	Window* window = nullptr;
	Image* imageFromHistogram = nullptr;
public:
	Histogram() { imageFromHistogram = nullptr; }

	void setWindow(Window* w) {  window = w; }
	void setImage(Image* i) { clearImage(); imageFromHistogram = i; }
	void drawHistogram();
	Image* getImage(){ return imageFromHistogram; }
	void clearImage() { imageFromHistogram = nullptr; }
};


class ErrorTextField
{
	Image* imageFromHistogram;
	Window* window = nullptr;
	sf::RectangleShape overlayDim;   // œciemnienie t³a
	sf::Text  overlayText;           // du¿y napis b³êdu
	bool show = false;

public:


	void setWindow(Window* w) { window = w; }
	void setImage(Image* i) { imageFromHistogram = i; }
	void initWrongFile(std::string textMessage);
	void setShow(bool a) { show = a; }
	bool getShow() { return show; }

	void draw();
};


class ComboBox
{
	Window* window = nullptr;
	sf::RectangleShape box;
	sf::Text selectedText; // to co se wybierzesza

	std::vector<sf::RectangleShape> optionBoxes;// to co se wybierzesz ale graficznie ;/
	std::vector<sf::Text> optionTexts;	// to co se wybierzesz ale graficznie ;/ - teksty
	int  selectedIndex = 0; // to co se wybierzesz nu,mer


	bool expanded = false;

	
	sf::Text textDescription; // to co zobaczymy nad
	

	public:

		


		void setWindow(Window* w) { window = w; }
		void create(float x, float y, float width, float height,
			const std::vector<std::string>& items, sf::Font& font);


		void draw();
		void handleClick(const sf::Vector2f& mousePos);

		int  getSelectedNumber() const;  


};

class TextFieldFiles {

	sf::Text textDescription;
	sf::RectangleShape background;
	std::vector<std::string> Text;
	Window* window = nullptr;
public:


	void setWindow(Window* w) { window = w; }

	void setTextField(int x, int y, int width, int heigh, sf::Font& font);

	void drawTextField();

	void makeTextField();
	

	std::vector<std::string> iterate();



};



class CheckBox
{
	Window* window = nullptr;
	sf::RectangleShape box;
	sf::Text label;
	bool checked = false;

public:
	void setWindow(Window* w) { window = w; }

	void create(float x, float y, const std::string& text, sf::Font& font)
	{
		box.setPosition(x, y);
		box.setSize({ 20.f, 20.f });
		box.setFillColor(sf::Color::White);
		box.setOutlineThickness(2.f);
		box.setOutlineColor(sf::Color::Black);

		label.setFont(font);
		label.setCharacterSize(18);
		label.setFillColor(sf::Color::Black);
		label.setString(text);
		label.setPosition(x + 30.f, y - 2.f);
	}

	void draw();
	

	void handleClick(const sf::Vector2f& mousePos)
	{
		if (box.getGlobalBounds().contains(mousePos))
		{
			checked = !checked;
		}
	}

	bool isChecked() const { return checked; }
};



class Window
{
private:
	
	sf::Texture backgroundTexture; // Przechowuje plik tlo.jpg
	sf::Sprite backgroundSprite;   // Wyœwietla t³o na ekranie


	friend class MainButton;
	friend class MainTextField;
	friend class Histogram;
	friend class ErrorTextField;
	friend class ComboBox;
	friend class TextFieldFiles;
	friend class CheckBox;
	


	// sta³e wysokoœci na których elementy siê znajduj¹

	const float yButton = 60;
	const float yControll = 700.f;

	//dla obrazow
	const float sizePicture = 400;
	const float xPicture = 550.f;
	const float xPictureBefore = 50;
	const float yPicture = 150;



	//check box
	CheckBox asmCheckBox;

	//okno z plikami

	TextFieldFiles allFiles;





	// dla wyboru ilosci watkow
	ComboBox threadsCombo;

	
	
	
	// okno b³¹d

	ErrorTextField fileError;
	ErrorTextField numberError;

	bool showWrongPath = false;

	

	




	// g³owne

	sf::RenderWindow* window; // g³owne okno programu
	sf::VideoMode videoMode; // parametry okna graficznego
	sf::Event ev;  // pojedyncze zdarzenie z kolejki SFML-a — u¿ywane do odczytywania, co robi u¿ytkownik.

	sf::Font font;

	void initVariable(); //prywata do inicjacji
	void initWindow();
	void initComb();
	//fragmenty geometryczne
	MainButton button2; // dla y=wystwietlenia
	MainButton button3; //dla zblurowania
	

	//pola tekstowe
	std::string currentFilePath = ""; //nowe
	// MainTextField text1; // tu wpisujesz nazwe pliku stare
	MainTextField text2;// kernel
	MainTextField text3; // odchylenie
	

	// histogram
	//Image* imageFromHistogram = nullptr;
	Histogram ilustratingBefor;
	Histogram ilustratingAfter;

	//obrazy do wyœwietlenia przed i po wow albo i nie

	SpriteWithTexture leftBefore;
	SpriteWithTexture rightBefore;








	//reszta
	bool hit(const sf::RectangleShape& r, sf::Vector2f p) const {
		return r.getGlobalBounds().contains(p);
	}



	

	int buttonAction = 0;


public:

	std::string OpenFileDialog();
	//funkcje dla ckeckboxa
	bool getUseAsm() const { return asmCheckBox.isChecked(); }


	//funkcje dla pola tekstowego
	void initViewFiles();


	//funckja dla threadsów
	int getThreadsFromCombo() const { return threadsCombo.getSelectedNumber(); }

	void setToErrorNumber(bool a) { numberError.setShow(a); }
	//funkcje dla histogramu
	Histogram& getHistAfter() { return ilustratingAfter; }
	Histogram& getHistBefore() { return ilustratingBefor; }
	//funkcje dla wizualizacji orbazów
	void initPhotoBefore(const std::string& path);
	void initPhotoAfter(const std::string& path);
	void windowRenderPhotos();

	//void drawHistogram( Image& img);

	//void setImageToDraw(Image* img) { this->imageFromHistogram = img; }
	//Image getImage() { return *imageFromHistogram; }
	
	
	void createTextFields();
	

	
	
	sf::Vector2u getWindowSize() const {
		return window->getSize();
	}


	void renderText(); //np miganie


	
	void TextEnterNormal(uint32_t ch);
	void TextEnterNumber(uint32_t ch);


	int getButtonAction();


	const bool getWindowIsOpen();

	
	void createButtons();




	Window();
	~Window();

	void update();
	void pollEvents();
	
	//void render();
	void render();

	std::string getTextValue();
	int getSize();
	int getOdchylenie();


	void fitSpriteToBox(sf::Sprite& spr, const sf::Texture& tex, float boxW, float boxH);

	void createErrorFields();

		void renderErrorFieldAndHistogram();


		bool readAndValidateKernelSigma(int kernel, int sigma);

		bool checkEmptyNumber();


	
};



