/*
    Projekt: Aplikacja z filtrem Gaussa (DLL ASM + DLL C++ + GUI SFML)
    Opis pliku:Plik implementuje klasę Window odpowiedzialną za tworzenie i obsługę graficznego interfejsu użytkownika w bibliotece SFML.

      Opis pliku:
        Plik zawiera implementacje pomocniczych klas GUI wykorzystywanych w klasie Window, m.in.:
          - MainButton        – przycisk z napisem,
          - MainTextField     – pola tekstowe do wpisywania danych,
          - Histogram         – rysowanie histogramu R,G,B,
          - ErrorTextField    – duży czerwony komunikat o błędach,
          - ComboBox          – wybór liczby wątków (dropdown),
          - TextFieldFiles    – pole z listą dostępnych plików JPG,
          - CheckBox          – wybór użycia ASM / C++.


          Elementy jedynie rysują i obsługują wejście.


    Autor: Kamil Musioł
    Semestr: Semestr zimowy 2024/2025
    */




#include "Window.h"




// część dla buttonów



/*
    
    Opis:
        Inicjalizuje przycisk GUI z podanym tekstem, rozmiarem i pozycją. Ustawia:
            - rozmiar i kolor prostokąta,
            - font i napis na przycisku,
            - wyśrodkowanie tekstu względem przycisku.

    Parametry:
        size          – rozmiar przycisku (szerokość, wysokość),
        x, y          – pozycja lewego górnego rogu,
        font          – czcionka użyta do tekstu,
        text          – napis wyświetlany na przycisku,
        charakterSize – rozmiar czcionki,
        fillColor     – kolor tekstu.
*/
void MainButton::createButton(const sf::Vector2f& size, float x, float y, const sf::Font& font, std::string text, int charakterSize, const sf::Color& fillColor)
{
    button.setSize(size);
    button.setPosition(x, y);
    button.setFillColor(sf::Color(100, 100, 250));

    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(charakterSize);
    buttonText.setFillColor(fillColor);

    textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    buttonText.setPosition(button.getPosition().x + button.getSize().x / 2.0f,
        button.getPosition().y + button.getSize().y / 2.0f);
}

//rysuje przycisk
void MainButton::drawButton()  {
    window->window->draw(button);
    window->window->draw(buttonText);

}

//zwraca prostokąt otaczający przycisk  - sprawdzamy tym czy kliknięto
sf::FloatRect MainButton::getGlobalBounds()
{
    return button.getGlobalBounds();
}

















// część dla texkstowych

//Ustawia grubość i kolor obramowania pola tekstowego w zależności od tego, czy pole ma fokus(czy użytkownik w nim pisze).
void MainTextField::renderText(float outlineFickness)
{
    textBox.setOutlineThickness(outlineFickness);
    textBox.setOutlineColor(textFocused ? sf::Color(180, 180, 255) : sf::Color(120, 120, 120));
}

//rysuje na oknie pole tkestowe
void MainTextField::drawText()
{
    window->window->draw(textDescription);
    window->window->draw(textBox);
    window->window->draw(textLabel);
   
}

/*
    Opis:
        Tworzy pojedyncze pole tekstowe wraz z opisem nad polem.
        Pole służy do wprowadzania tekstu (nazwa pliku, liczby, itp.).

    Parametry:
        x, y    – pozycja lewego górnego rogu pola,
        width   – szerokość prostokąta,
        tekst   – opis pola 
        font    – czcionka użyta do opisu i wartości.
*/



void MainTextField::createTextField(float x, float y, float width, std::string tekst, sf::Font& font)
{
    // ramka
    textBox.setPosition(x, y);
    textBox.setSize({ width, 40.f });
    textBox.setFillColor(sf::Color(30, 30, 30));
    textBox.setOutlineThickness(2.f);
    textBox.setOutlineColor(sf::Color(120, 120, 120));

    // napis

    textLabel.setFont(font);              
    textLabel.setCharacterSize(18);
    textLabel.setFillColor(sf::Color::White);
    textLabel.setPosition(x + 10.f, y + 8.f);

    textValue.clear();                    
    textLabel.setString("");              




    textDescription.setFont(font);
    textDescription.setCharacterSize(18);
    textDescription.setFillColor(sf::Color::Black);


    textDescription.setString(tekst);


    //textDescription.setString("Place here name of the file (must be jpg):");
    textDescription.setPosition(x, y - 30.f); 
}



// część dla hisotgramu




/*
  
    Opis:
        Rysuje histogram kolorów R, G, B na podstawie danych z obiektu Image. Dla każdego kanału (maksymalnie 3) rysowanych jest 256 słupków,
        których wysokość odpowiada liczbie pikseli o danej intensywności.

        Skalowanie (scaleY) dobierane jest dynamicznie na podstawie  całkowitej liczby pikseli, aby histogram był czytelny zarówno  dla małych, jak i dużych obrazów.
  
*/
void Histogram::drawHistogram()
{
    if (imageFromHistogram == nullptr || imageFromHistogram->colorHistogram.empty()) return;

    // 1. Ustawienia geometryczne
    float startX = 100.f;  // Margines z lewej
    // Histogram kończy się 50px od dołu okna
    float baseY = window->window->getSize().y - 50.f;

    // NOWOŚĆ: Maksymalna wysokość słupków w pikselach (np. 150px)
    float maxDrawHeight = 150.f;

    float barWidth = 3.f;  // Szerokość pojedynczego słupka

    // 2. Znajdź największą wartość (peak) we wszystkich kanałach
    // To pozwoli nam dopasować skalę idealnie do okna
    int maxVal = 0;
    for (const auto& channelData : imageFromHistogram->colorHistogram)
    {
        for (int val : channelData)
        {
            if (val > maxVal) maxVal = val;
        }
    }

    // Zabezpieczenie przed dzieleniem przez zero (gdyby obraz był pusty)
    if (maxVal == 0) maxVal = 1;


    sf::Color colors[3] = {
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue
    };

    // 3. Rysowanie
    for (int ch = 0; ch < imageFromHistogram->colorHistogram.size(); ++ch)
    {
        const auto& hist = imageFromHistogram->colorHistogram[ch];

        for (int i = 0; i < 256; ++i)
        {
            // NOWA FORMUŁA SKALOWANIA:
            // (wartość / max_wartość) * stała_wysokość
            float height = ((float)hist[i] / (float)maxVal) * maxDrawHeight;

            sf::RectangleShape bar(sf::Vector2f(barWidth, -height));

            // Przesunięcie każdego koloru lekko, żeby się nie nakładały idealnie (opcjonalne)
            // lub rysowanie obok siebie. Tutaj zostawiam Twoją logikę (nakładanie/przesunięcie o 1px)
            bar.setPosition(startX + i * (barWidth + 1), baseY);

            // Ustawiamy przezroczystość (alpha = 150), żeby było widać słupki pod spodem
            sf::Color c = colors[ch];
            c.a = 150;
            bar.setFillColor(c);

            window->window->draw(bar);
        }
    }
}

// część dla errorów



/*
    
    Opis:  Inicjalizuje duży czerwony napis błędu wyświetlany przy dolnej krawędzi okna. 
          
*/


void ErrorTextField::initWrongFile(std::string textMessage)
{
    overlayText.setFont(window->font);
    overlayText.setCharacterSize(80);
    overlayText.setFillColor(sf::Color::Red);
    overlayText.setStyle(sf::Text::Bold);
    overlayText.setString(textMessage);

    // wyśrodkuj
    sf::FloatRect bounds = overlayText.getLocalBounds();
    overlayText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    overlayText.setPosition(window->videoMode.width / 2.f, window->videoMode.height - bounds.height / 2.f - 100.f);




}

void ErrorTextField::draw()
{
    this->window->window->draw(overlayText);
}


// dla comboboxa (threadsy)



/*
    Opis:
        Tworzy rozwijane pole (combobox) z listą opcji tekstowych.
        Używane do wyboru liczby wątków filtracji (1–7).

    Parametry:
        x, y       – pozycja głównego boxa,
        width, height – wymiary głównego pola,
        items      – lista opcji 
        font       – czcionka do tekstu.
*/
void ComboBox::create(float x, float y, float width, float height,
    const std::vector<std::string>& items, sf::Font& font)
{
    // główny box
    box.setPosition(x, y);
    box.setSize({ width, height });
    box.setFillColor(sf::Color(230, 230, 230));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(120, 120, 120));

    // tekst aktualnie wybranej opcji
    selectedText.setFont(font);
    selectedText.setCharacterSize(18);
    selectedText.setFillColor(sf::Color::Black);

    if (!items.empty())
        selectedText.setString(items[0]);  // domyślnie pierwsza opcja

    // ustaw pozycję tekstu w środku boxa
    sf::FloatRect tb = selectedText.getLocalBounds();
    selectedText.setPosition(
        x + 10.f,
        y + (height - tb.height) / 2.f - 5.f
    );

    // przygotowanie boxów i tekstów dla opcji (pod spodem)
    optionBoxes.clear();
    optionTexts.clear();

    const float optionHeight = height;

    for (std::size_t i = 0; i < items.size(); ++i)
    {
        sf::RectangleShape optBox;
        optBox.setSize({ width, optionHeight });
        optBox.setPosition(x, y + height + i * optionHeight);
        optBox.setFillColor(sf::Color(240, 240, 240));
        optBox.setOutlineThickness(1.f);
        optBox.setOutlineColor(sf::Color(150, 150, 150));

        sf::Text optText;
        optText.setFont(font);
        optText.setCharacterSize(18);
        optText.setFillColor(sf::Color::Black);
        optText.setString(items[i]);

        sf::FloatRect ob = optText.getLocalBounds();
        optText.setPosition(
            optBox.getPosition().x + 10.f,
            optBox.getPosition().y + (optionHeight - ob.height) / 2.f - 5.f
        );

        optionBoxes.push_back(optBox);
        optionTexts.push_back(optText);
    }


    // część dla tekstu nad wow

    textDescription.setFont(font);
    textDescription.setCharacterSize(18);
    textDescription.setFillColor(sf::Color::Black);


    textDescription.setString("Ilosc watkow");


    //textDescription.setString("Place here name of the file (must be jpg):");
    textDescription.setPosition(x, y - 30.f); // 30px nad polem

   
}



//rysowanie comboboxa
void ComboBox::draw()
{
    if (!window) return;

    // rysuj główny box
    window->window->draw(box);
    window->window->draw(selectedText);
    window->window->draw(textDescription);
    
    // jeśli rozwinięty – rysuj opcje pod spodem
    if (expanded)
    {
        for (std::size_t i = 0; i < optionBoxes.size(); ++i)
        {
            window->window->draw(optionBoxes[i]);
            window->window->draw(optionTexts[i]);
        }
    }
}

/*
 Obsługuje kliknięcia:
            - klik w główny box → rozwinięcie/zwiniecie listy,
            - klik w jedną z opcji → ustawienie selectedIndex i zamknięcie listy,
            - klik poza -> zwinięcie listy.

*/
void ComboBox::handleClick(const sf::Vector2f& mousePos)
{
    // 1. klik w główny borozwiń/zwiń
    if (box.getGlobalBounds().contains(mousePos))
    {
        expanded = !expanded;
        return;
    }

    // 2. jeśli rozwinięty – sprawdź czy kliknięto którąś z opcji
    if (expanded)
    {
        for (std::size_t i = 0; i < optionBoxes.size(); ++i)
        {
            if (optionBoxes[i].getGlobalBounds().contains(mousePos))
            {
                selectedIndex = static_cast<int>(i);
                selectedText.setString(optionTexts[i].getString());
                expanded = false;
                return;
            }
        }

        // kliknięto poza opcjami schowaj
        expanded = false;
    }
}
//zwraca wartosc komboboxa
int ComboBox::getSelectedNumber() const
{
    if (optionTexts.empty()) return 1;

    // tekst np. "1", "2", "3"...
    const sf::String txt = optionTexts[selectedIndex].getString();
    try {
        return std::stoi(txt.toAnsiString());
    }
    catch (...) {
        return 1; // fallback
    }
}




/// dla fieldu z plikami

//przegląda bieżący folder i zwraca listę plików
std::vector<std::string> TextFieldFiles::iterate()
{
    fs::path current = fs::current_path();

    std::vector<std::string> a;
    std::string b;
    a.push_back("Lista dostepnych plikow: ");

    for (const auto& entry : fs::directory_iterator(current)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".jpg" || ext == ".jpeg" || ext == ".JPG" || ext == ".JPEG") {
                b = " - " + entry.path().filename().string();
                a.push_back(b);
            }
        }
    }

    return a;
}


//buduje tekstrową listę plików dostępnych
void TextFieldFiles::makeTextField()
{
    textDescription.setString("");
    std::vector<std::string> a = iterate();
    for (std::string b : a)
    {
        std::string current = textDescription.getString();
        current = current + b + "\n";
        textDescription.setString(current);
    }
}




//inicjuje obszar w którym pole tekstowe będzie wyświetlane

void TextFieldFiles::setTextField(int x, int y, int width, int height, sf::Font& font)
{


    background.setPosition(x, y);
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(sf::Color(30, 30, 30));
    background.setOutlineThickness(2.f);
    background.setOutlineColor(sf::Color::Black);



    this->textDescription.setCharacterSize(20);
    this->textDescription.setFont(font);
    this->textDescription.setFillColor(sf::Color::White);


    sf::FloatRect bounds = textDescription.getLocalBounds();

    // origin tylko w poziomie (środek X)
    // Y = 0 → tekst zaczyna rysować od górnej krawędzi
    textDescription.setOrigin(bounds.left + bounds.width / 2.f, 0.f);

    // wyśrodkowanie w poziomie, ale pionowo ustawione na górze
    textDescription.setPosition(x , y);




};


//rysuje
void TextFieldFiles::drawTextField()
{
    window->window->draw(this->background);
    window->window->draw(this->textDescription);
}


// vheck box

//rysuje checkobxa i etykiete tekstową
void CheckBox::draw()
{
    window->window->draw(this->box);
    window->window->draw(this->label);




    if (checked)
    {
        sf::RectangleShape mark;
        mark.setSize({ 14.f, 14.f });
        mark.setFillColor(sf::Color::Black);
        mark.setPosition(box.getPosition().x + 3,
            box.getPosition().y + 3);
        window->window->draw(mark);
    }
}