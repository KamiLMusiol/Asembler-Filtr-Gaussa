
/*
    Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
    Opis pliku:Plik implementuje klasę Window odpowiedzialną za tworzenie i obsługę graficznego interfejsu użytkownika w bibliotece SFML. 

    Klasa zarządza:
     - oknem programu,
     - przyciskami (Open file, Blur the file),
     - polami tekstowymi (nazwa pliku, kernel, sigma),
     - polem błędów,
     - checkboxem wyboru implementacji (ASM / C++),
     - comboboxem wyboru liczby wątków,
     - wyświetlaniem obrazów przed i po filtracji,
     - renderowaniem histogramu,
     - obsługą zdarzeń klawiatury i myszy.





    Autor: Kamil Musioł
    Semestr: Semestr zimowy 2024/2025
    */



#include "Window.h"
#include <string>
#include <commdlg.h>


std::string Window::OpenFileDialog()
{
    char filename[MAX_PATH] = ""; // Bufor na nazwę pliku

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = this->window->getSystemHandle();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "JPG Images\0*.jpg;*.jpeg\0All Files\0*.*\0"; //tylko jpg
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(filename);
    }

    return std::string(); // Zwraca pusty string, jeśli anulowano
}

Window::Window()
{

	this->initVariable();
	this->initWindow();

	



};


//tworzy przyciski, pozycjonuje je 
void Window::createButtons()
{
    
  

    float btnW = 260.f;

    // Pozycjonowanie pod obrazkami
    float x1 = xPictureBefore + (sizePicture / 2.f) - (btnW / 2.f);
    float x2 = xPicture + (sizePicture / 2.f) - (btnW / 2.f);

    button2.createButton(sf::Vector2f(btnW, 50.f), x1, yButton, font, "Open file", 20, sf::Color::White);
    button2.setWindow(this);

    button3.createButton(sf::Vector2f(btnW, 50.f), x2, yButton, font, "Run Gauss Blur", 20, sf::Color::White);
    button3.setWindow(this);



}
//towrzy i pozycjnonuje pola tekstowe
void Window::createTextFields()
{
    // Kernel po lewej
    text2.createTextField(150.f, yControll, 60.f, "Kernel Size", font);
    text2.setWindow(this);

    // Sigma obok Kernela
    text3.createTextField(350.f, yControll, 60.f, "Sigma", font);
    text3.setWindow(this);
   
   
}


//tworzy i pozycjonuje pole gdzie będzie wyświetlana inofmracja o błędzie
void Window::createErrorFields()
{
    fileError.setWindow(this);
    fileError.initWrongFile("Brak nazwy obrazu");
    fileError.setShow(false);


    numberError.setWindow(this);
    numberError.initWrongFile("Zle podane liczby");
    numberError.setShow(false);
    
}


void Window::initVariable()
{
	this->window = nullptr;
}

//tworzy komboboxa
void Window::initComb()
{
    std::vector<std::string> values = { "1","2","3","4","8","16","32","64"}; // 
    threadsCombo.setWindow(this);

    // Ustawiamy obok Sigmy
    threadsCombo.create(550.f, yControll, 80.f, 40.f, values, font);
}

//towrzy pole tekstowe wyświetlające liczbę dostępnych plików
void Window::initViewFiles()
{
    allFiles.setTextField(1100, 200, 250, 200, this->font);
    allFiles.setWindow(this);
    allFiles.makeTextField();
}


//tworzy głowne okno i inicjuje elementy
void Window::initWindow()
{
    
    // cale okno zmien potem na caly ekran
	this->videoMode.height = 1200;
	this->videoMode.width = 1400;
    font.loadFromFile("arial.ttf");
	this->window = new sf::RenderWindow(this->videoMode, "Gauss", sf::Style::Titlebar | sf::Style::Close);


    if (!backgroundTexture.loadFromFile("tlo.jpg"))
    {
        // Opcjonalnie: obsługa błędu, np. wypisanie w konsoli
        // std::cout << "Nie udalo sie wczytac tlo.jpg!" << std::endl;
    }

    // 2. Przypisz teksturę do sprite'a
    backgroundSprite.setTexture(backgroundTexture);

    // 3. Przeskaluj tło, żeby idealnie pasowało do okna (rozciągnij)
    // Pobieramy rozmiar okna i rozmiar obrazka
    float windowW = static_cast<float>(videoMode.width);
    float windowH = static_cast<float>(videoMode.height);

    // Zabezpieczenie, żeby nie dzielić przez zero, jeśli obrazek się nie wczytał
    sf::Vector2u texSize = backgroundTexture.getSize();
    if (texSize.x > 0 && texSize.y > 0)
    {
        float scaleX = windowW / static_cast<float>(texSize.x);
        float scaleY = windowH / static_cast<float>(texSize.y);
        backgroundSprite.setScale(scaleX, scaleY);
    }


    ilustratingBefor.setWindow(this);
    ilustratingAfter.setWindow(this);

    asmCheckBox.setWindow(this);
    asmCheckBox.create(800.f, yControll + 10.f, "Use Assembler (ASM)", font); // +10 żeby wyrównać tekst z polami

    createTextFields();
    

    initComb();
  //  initViewFiles(); stare
    
    createErrorFields();
   
    
   

	createButtons();
}




Window::~Window()
{
	delete this->window;
};


//obsługuje wpisywanie tekstu do pola nazwa pliku (backspac, częsc ascii)
void Window::TextEnterNormal(uint32_t ch)
{// stare
    /*
    if (ch == 8) { // Backspace
        if (!text1.getValue().empty()) text1.getValue().pop_back();
    }
    else if (ch == 13) {
        // Enter – na razie nic; możesz tu wywołać akcję (np. Load)
    }
    else if (ch >= 32 && ch < 127) {
        // drukowalne ASCII (w tym \ i :)
        text1.getValue().push_back(static_cast<char>(ch));
    }
    text1.getLabel().setString(text1.getValue());
    */
}

//obsługuje pola tekstowe do których można wpisywać tylko liczby 
void Window::TextEnterNumber(uint32_t ch)
{
    // pole 2
    if (text2.getFocus()) {
        if (ch == 8) { // Backspace
            if (!text2.getValue().empty()) text2.getValue().pop_back();
        }
            else if (ch >= '0' && ch <= '9') {
            text2.getValue().push_back(static_cast<char>(ch));
            
        }
        text2.getLabel().setString(text2.getValue());
    }

    // pole 3
    if (text3.getFocus()) {
        if (ch == 8) {
            if (!text3.getValue().empty()) text3.getValue().pop_back();
        }
       
        else if (ch >= '0' && ch <= '9') {
            text3.getValue().push_back(static_cast<char>(ch));
            
        }
        text3.getLabel().setString(text3.getValue());
    }
   
}






//obsługuje zdarzenia w sfml:zamknięcie okna, kliknięcie myszy, wybó pól tekstowych buttonów comboboxów, wpisanie tekstu
void Window::pollEvents()
{
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case::sf::Event::Closed:
			this->window->close(); break;
		case::sf::Event::KeyPressed:
			if (this->ev.key.code == sf::Keyboard::Escape) this->window->close(); break;
        case sf::Event::MouseButtonPressed:
            if (ev.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f m{ (float)ev.mouseButton.x, (float)ev.mouseButton.y };

              //  text1.setFocus(hit(text1.getBox(), m));    // focus gdy kliknięto w pole
                text2.setFocus(hit(text2.getBox(), m));
                text3.setFocus(hit(text3.getBox(), m));


                // Wewnątrz Window::pollEvents -> case sf::Event::MouseButtonPressed:

                if (button2.getGlobalBounds().contains(m)) {
                   

                    //
                    std::string path = OpenFileDialog(); // Otwórz okno dialogowe

                    if (!path.empty()) // Jeśli użytkownik coś wybrał
                    {
                       
                       // text1.getValue() = path;            // Aktualizacja zmiennej string w polu stare
                        //text1.getLabel().setString(path);   // Aktualizacja widocznego napisu SFML teraz nie ma stare
                         
                        // wczytać ten plik
                        this->currentFilePath = path;
                        buttonAction = 2;
                    }
                }
                else if (button3.getGlobalBounds().contains(m)) {
                    buttonAction = 3;
                }
                threadsCombo.handleClick(m);
                asmCheckBox.handleClick(m);
            }            break;

        case sf::Event::TextEntered:

            uint32_t ch = ev.text.unicode;
           // if (text1.getFocus()) TextEnterNormal(ch); //tekst plik stare
            if (text2.getFocus() || text3.getFocus()) TextEnterNumber(ch); // pola liczbowe
                
                

            
                
           

            
            


		}

	}
	
}


//funkcja cykliczna wywołująca poll event
void Window::update()
{
	this->pollEvents();
	

};


//renderuje pola tekstowe powoduje migający kursor
void Window::renderText()
{
   
   
    text2.renderText(2.f);
    //text1.renderText(2.f);
    text3.renderText(2.f);
   
 


    // Migający kursor w każdym aktywnym polu
    sf::Clock clk1, clk2, clk3;

    auto applyCaret = [](bool focused, std::string& val, sf::Text& label, sf::Clock& clk)
        {
            const int ms = clk.getElapsedTime().asMilliseconds();
            const bool visible = ((ms / 500) % 2) == 0; // co 0.5 s
            label.setString(focused ? (val + (visible ? "|" : " ")) : val);
        };

  //  applyCaret(text1.getFocus(), text1.getValue(), text1.getLabel(), text1.getClock());
    applyCaret(text2.getFocus(), text2.getValue(), text2.getLabel(), text2.getClock());
    applyCaret(text3.getFocus(), text3.getValue(), text3.getLabel(), text3.getClock());

    

   // text1.drawText();
    text2.drawText();
    text3.drawText();
   
   
}


//rysuje obrazy
void Window::windowRenderPhotos()
{
    if (leftBefore.canBeShown == true)
    {
        this->window->draw(leftBefore.sprite);
    }
    if (rightBefore.canBeShown == true)
    {
        this->window->draw(rightBefore.sprite);
    }
}



//wyświetla błąd w przypadku błędu lub rysuje histogram
void Window::renderErrorFieldAndHistogram()
{
    
     if (fileError.getShow())
    {
        fileError.draw();
    }
     else if (numberError.getShow())
     {
         numberError.draw();
     }
    else
    {
        if (ilustratingAfter.getImage() != nullptr) {
            ilustratingAfter.drawHistogram();
        }
    }
   

}


//gunkcja rysująca gui: czyści ekran, rysuje elementy, wyświetla okno
void Window::render()
{

   

	this->window->clear(sf::Color(255,255,255));


    this->window->draw(this->backgroundSprite);

	//tu rysuj obikety

   // allFiles.makeTextField(); stare
    button2.drawButton();
    button3.drawButton();
	

    renderText();
	
    threadsCombo.draw();
    asmCheckBox.draw();

    

    this->renderErrorFieldAndHistogram();

    this->windowRenderPhotos();
    
   // this->allFiles.drawTextField();
   
    this->window->display();
    

};




//mówi czy okno jest otwarte

const bool Window::getWindowIsOpen()
{
	return this->window->isOpen();
}


//zwraca akje użytkownika (2 to zwykłe foto 3 to blur)
int Window::getButtonAction()
{
    int a = buttonAction;
    buttonAction = 0;   // reset po odczycie
    return a;
}



std::string Window::getTextValue() 
{
    return currentFilePath;
}
int Window::getSize()
{
    
    return std::stoi(text2.getValue());
}

int Window::getOdchylenie()
{

    return std::stoi(text3.getValue());
}

//skaluje obraz by zmieścił się w prostokącie

void Window::fitSpriteToBox(sf::Sprite& spr, const sf::Texture& tex, float boxW, float boxH)
{
    auto sz = tex.getSize();
    if (sz.x == 0 || sz.y == 0) return;
    float sx = boxW / static_cast<float>(sz.x);
    float sy = boxH / static_cast<float>(sz.y);
    float s = std::min(sx, sy);          
    spr.setScale(s, s);
}

//pobiera plik skaluje go i ustawia pozycje (obraz po lewej stornie)
void Window::initPhotoBefore(const std::string& path)
{
    leftBefore.clearTexture();
    if (!leftBefore.texture.loadFromFile(path)) { leftBefore.canBeShown = false; fileError.setShow(true); return; }
    else
    {
        fileError.setShow(false);
       
    }
    
    leftBefore.texture.setSmooth(true);
    leftBefore.sprite.setTexture(leftBefore.texture);

    
    fitSpriteToBox(leftBefore.sprite, leftBefore.texture, sizePicture, sizePicture);

    leftBefore.sprite.setPosition(xPictureBefore, yPicture);
    leftBefore.canBeShown = true;
}
//pobiera plik skaluje go i ustawia pozycje (obraz po prawej stornie)
void Window::initPhotoAfter(const std::string& path)
{


    rightBefore.clearTexture();

    
   
        if (!rightBefore.texture.loadFromFile(path)) {
            rightBefore.canBeShown = false;
            fileError.setShow(true);
            return;
        }
        else
        {
            fileError.setShow(false);
        }

       
        

    

    
    

    rightBefore.texture.setSmooth(true);
    rightBefore.sprite.setTexture(rightBefore.texture);
    fitSpriteToBox(rightBefore.sprite, rightBefore.texture, sizePicture, sizePicture);
    rightBefore.sprite.setPosition(xPicture, yPicture);
    rightBefore.canBeShown = true;
  
}



//waliduje poprawnosc kerela (kernel>0, kernel nieparzysty, sigma>0, pola nie są puste)
bool Window::readAndValidateKernelSigma(int kernel, int sigma)
{
    //puste
    if (text2.getValue().empty() || text3.getValue().empty()) {
        
        numberError.setShow(true);
        return false;
    }
   
    
    

    // 3) warunki: kernel > 0, NIEparzysty; sigma > 0
    if (kernel <= 0 || (kernel % 2) == 0 || sigma <= 0) {
        numberError.setShow(true);
        return false;
    }

    // OK

    numberError.setShow(false);
    fileError.setShow(false);
    return true;
}



//sprawdza czy pola tekstowe nie są puste
bool Window::checkEmptyNumber()
{
    int kernel;
    int sigma;
    try {
        kernel = std::stoi(text2.getValue());  // "Wielkosc kernela"
        sigma = std::stoi(text3.getValue());  // "Odchylenie standardowe"
    }
    catch (...) {
        
        return true;
    }

    return false;
}




