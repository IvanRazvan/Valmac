#include "CommandBox.h"
#include "Valmac.h"
#include "LoadButton.h"
#include "SaveButton.h"
#include "InitButton.h"
#include "RunButton.h"
#define DistanceBetweenRows	(39)
#define PixelSize (6)

// This function adds an empty line after the codePointer
void CommandBox::AddLine()
{
    // We push the codePointer to the end of the current line
    while (m_codePointer < m_codeString.size() && m_codeString.at(m_codePointer) != '\n')
        m_codePointer++;

    if (m_codePointer >= m_codeString.size())
        return;

    // We add a new line and move the following lines forward in the string
    for (int i = 0; i < 7; i++)
        m_codeString += ' ';
    
    for (int i = m_codeString.size() - 1; i > m_codePointer; i--)
        m_codeString.at(i) = m_codeString.at(i - 7);

    char newLine[6] = { '0', 'x', ' ', ' ', ' ', ' ' };
    for (int i = 0; i < 6; i++)
        m_codeString.at(m_codePointer + 1 + i) = newLine[i];

    m_numberOfLines++;
}

// This function adds a key into the codeString
void CommandBox::AddKey(const char  key)
{
    // If we are changing some code
    if (m_codePointer < m_codeString.size())
    {
        // If we are at the end of the line, we push the pointer to the next line
        if(m_codeString.at(m_codePointer) == '\n')
            m_codePointer += 3;

        // We add the they to the code
        if(m_codePointer < m_codeString.size())
            m_codeString.at(m_codePointer) = key;
        else
            m_codeString += key;
    }
    else
    {
        // If we are at the end of the line, we create a new line
        if ((m_codeString.size() + 1) % 7 == 0)
        {
            m_codePointer += 3;
            m_numberOfLines++;
            m_codeString += "\n0x";
        }
        
        // We add the they to the code
        m_codeString += key;
    }
    
    // We move the codePointer to the next location
    m_codePointer++;

    if(m_codePointer < m_codeString.size() && m_codeString.at(m_codePointer) == '\n')
        m_codePointer += 3;
}

// This function deletes a line of code from codeString
void CommandBox::DeleteKey()
{
    // If there is more than 1 line 
    if (m_codeString.size() > 7)
    {
        // If we are inside the code
        if (m_codePointer < m_codeString.size())
        {
            // We delete the line and move the pointer to the previous line
            while (m_codeString.at(m_codePointer) != 'x')
                m_codePointer--;
            m_codeString.erase(m_codePointer - 1, 7);
            m_numberOfLines--;
            m_codePointer -= 3;
        }
        else
        {
            // We delete the last line and move the pointer to the previous line
            while (m_codeString.at(m_codeString.size() - 1) != 'x')
                m_codeString.erase(m_codeString.size() - 1, 1);
            m_codeString.erase(m_codeString.size() - 1, 1);
            m_codeString.erase(m_codeString.size() - 1, 1);
            m_codeString.erase(m_codeString.size() - 1, 1);
            m_numberOfLines--;
            m_codePointer = m_codeString.size();
        }
    }
    else
    {
        // We delete the only line
        ClearCode();
    }

    if (m_codePointer < 2)
        m_codePointer = 2;
}

// This function clears the code from codeString
void CommandBox::ClearCode()
{
    m_codeString = "0x";
    m_codePointer = 2;
    m_numberOfLines = 0;
}

// This function returns number written in base 16 as a string
std::string CommandBox::NumberToHexString(int number)
{
    if (number == 0)
        return "0";

    std::string digits = "0123456789ABCDEF";
    std::string result;

    while (number > 0)
    {
        result = digits[number % 16] + result;
        number /= 16;
    }

    return result;
}

// This function returns a number read from a string in base 16
uint16_t CommandBox::HexStringToNumber(std::string& text, int pos)
{
    uint16_t number = 0;
    for (int i = 0; i < 4; i++)
    {
        if (text.at(pos + i) >= '0' && text.at(pos + i) <= '9')
            number += pow(16, 3 - i) * (text.at(pos + i) - '0');
        else
            number += pow(16, 3 - i) * (text.at(pos + i) - 'A' + 10);
    }
    return number;
}

// This function renders HUD
void CommandBox::RenderHUD(sf::RenderWindow& window)
{
    window.draw(m_HUD);
}

// This function renders the column of registers
void CommandBox::RenderRegisters(sf::RenderWindow& window)
{
    std::string registersString;

    for (int i = 0; i < 16; i++)
        registersString += "V[" + NumberToHexString(i) + "]=" + NumberToHexString(m_valmac->V[i]) + '\n';

    sf::Text registersText;
    registersText.setFont(m_font);
    registersText.setCharacterSize(40);
    registersText.setPosition(655, 170);
    registersText.setString(registersString);
    window.draw(registersText);
}

// This function renders the code
void CommandBox::RenderCode(sf::RenderWindow& window)
{
    m_memoryIndicatorsString = "";
    for (uint16_t i = 0; i <= m_numberOfLines; i++)
    {
        m_memoryIndicatorsString += NumberToHexString(0x200 + 2 * i);
        m_memoryIndicatorsString += '\n';
    }
    m_memoryIndicatorsText.setString(m_memoryIndicatorsString);


    window.draw(m_memoryIndicatorsText);
    window.draw(m_codeText);
}

// This function renders PC, I and the timers
void CommandBox::RenderPCTimersI(sf::RenderWindow& window)
{
    sf::Vector2f pos(1020, 192);
    sf::Text text;
    text.setFont(m_font);
    text.setCharacterSize(40);
    text.setPosition(pos);
    text.setString("PC=" + NumberToHexString(m_valmac->PC));
    window.draw(text);

    text.setPosition(pos + sf::Vector2f(0, DistanceBetweenRows));
    text.setString(" I=" + NumberToHexString(m_valmac->I));
    window.draw(text);

    text.setCharacterSize(30);

    text.setPosition(pos + sf::Vector2f(0, 2 * DistanceBetweenRows));
    text.setString("Delay=" + NumberToHexString(m_valmac->delay_timer));
    window.draw(text);

    text.setPosition(pos + sf::Vector2f(0, 3 * DistanceBetweenRows));
    text.setString("Sound=" + NumberToHexString(m_valmac->sound_timer));
    window.draw(text);
}

// This function renders the stack
void CommandBox::RenderStack(sf::RenderWindow& window)
{
    sf::Text stackText;
    stackText.setFont(m_font);
    stackText.setCharacterSize(40);
    for (int i = 0; i < 16 && m_valmac->stack[i] != 0; i++)
    {
        stackText.setPosition(1360, 750 - i * DistanceBetweenRows);
        stackText.setString(NumberToHexString(m_valmac->stack[i]));
        window.draw(stackText);
    }
}

// This function renders pixels on the emulator screen, each 'pixel' is 6x6 pixels
void CommandBox::RenderScreen(sf::RenderWindow& window)
{
    sf::Vector2f startPos(913, 418);

    sf::RectangleShape background;
    
    // We draw the screen using two overlapping rectangles
    background.setSize(sf::Vector2f(66 * PixelSize, 34 * PixelSize));
    background.setFillColor(sf::Color::White);
    background.setPosition(startPos.x - PixelSize, startPos.y - PixelSize);
    window.draw(background);

    background.setSize(sf::Vector2f(64 * PixelSize, 32 * PixelSize));
    background.setFillColor(sf::Color::Black);
    background.setPosition(startPos.x, startPos.y);
    window.draw(background);

    // We render each active pixel
    sf::RectangleShape pixel;
    pixel.setSize(sf::Vector2f(PixelSize, PixelSize));
    pixel.setFillColor(sf::Color::White);
    for (int i = 0; i < 64 * 32; i++)
    {
        if (m_valmac->gfx[i])
        {
            pixel.setPosition(startPos.x + (i % 64) * PixelSize, startPos.y + (i / 64) * PixelSize);
            window.draw(pixel);
        }
    }
}

// This function renders the keypad
void CommandBox::RenderKeypad(sf::RenderWindow& window)
{
    int keys[16] = { 1,2,3,12,4,5,6,13,7,8,9,14,10,0,11,15 };
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(38,38));
    rectangle.setFillColor(sf::Color(0, 0, 0, 200));
   
    for(int i=0;i<16;i++)
        if (!m_valmac->keypad[keys[i]])
        {
            rectangle.setPosition(1016 + (i % 4) * 50, 640 + (i / 4) * 40);
            window.draw(rectangle);
        }
}

// This function renders the flickering codePointer
void CommandBox::RenderCodePointer(sf::RenderWindow& window)
{
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(32, 35));
    
    if (time(NULL) % 2)
        rectangle.setFillColor(sf::Color(255, 255, 255, 150));
    else
        rectangle.setFillColor(sf::Color(0, 0, 0, 150));

    rectangle.setPosition(m_textPosition.x + 1 + (m_codePointer % 7) * 31, m_textPosition.y + 8 + (m_codePointer/7) * DistanceBetweenRows);
    window.draw(rectangle);
}

// The constructor sets up te variables and the reference to the emulator
CommandBox::CommandBox(Valmac* valmac)
{
    m_valmac = valmac;
    m_valmac->initialize();

    m_codePointer = 2;
    m_numberOfLines = 0;
    m_codeString = "0x";

    if (!m_font.loadFromFile("_font.ttf"))
    {
        std::cout << "no font!";
        system("pause");
    }

    if (!m_HUDTexture.loadFromFile("_HUD.png"))
    {
        std::cout << "no HUD texture!";
        system("pause");
    }

    m_loadButton = std::make_shared<LoadButton>(this, sf::Vector2f(185, 830), sf::Vector2f(130, 52));
    m_saveButton = std::make_shared<SaveButton>(this, sf::Vector2f(350, 830), sf::Vector2f(130, 52));
    m_initButton = std::make_shared<InitButton>(this, sf::Vector2f(942, 820), sf::Vector2f(150, 60));
    m_runButton = std::make_shared<RunButton>(this, sf::Vector2f(1120, 820), sf::Vector2f(150, 60));

    m_HUD.setSize(sf::Vector2f(1600, 900));
    m_HUD.setTexture(&m_HUDTexture);

    m_textPosition = sf::Vector2f(307, 9 + 2 * DistanceBetweenRows);
    m_originalTextPosition = m_textPosition;

    m_codeText.setFont(m_font);
    m_codeText.setCharacterSize(40);
    m_codeText.setPosition(m_textPosition);
    m_codeText.setString(m_codeString);

    m_memoryIndicatorsText.setFont(m_font);
    m_memoryIndicatorsText.setCharacterSize(40);
    m_memoryIndicatorsText.setPosition(m_textPosition - sf::Vector2f(140,0));
    m_memoryIndicatorsText.setString(m_memoryIndicatorsString);
}

// This function computes a new location for the code based on the mouse scroll
void CommandBox::ComputeScroll(float scroll)
{
    if (scroll > 0)
        m_textPosition.y -= DistanceBetweenRows;

    if (scroll < 0)
        m_textPosition.y += DistanceBetweenRows;

    m_codeText.setPosition(m_textPosition);
    m_memoryIndicatorsText.setPosition(m_textPosition - sf::Vector2f(130, 0));
}

// This function adds or removes from the code depending on the keyboard input
void CommandBox::ComputeInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
    {
        AddKey('0');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    {
        AddKey('1');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    {
        AddKey('2');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
    {
        AddKey('3');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
    {
        AddKey('4');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
    {
        AddKey('5');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
    {
        AddKey('6');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
    {
        AddKey('7');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
    {
        AddKey('8');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
    {
        AddKey('9');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        AddKey('A');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
    {
        AddKey('B');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
    {
        AddKey('C');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        AddKey('D');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        AddKey('E');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    {
        AddKey('F');
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
    {
        DeleteKey();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    {
        AddLine();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
    {
        ClearCode();
    }

    m_codeText.setString(m_codeString);
}

// This function takes the written code and compiles it.
void CommandBox::Compile(std::vector<uint16_t>& code)
{
    code.clear();

    // The first error check looks for spaces left inside the code
    bool emptyInstructions = false;
    for (int i = 0; i < m_codeString.size() && !emptyInstructions; i++)
        if (m_codeString.at(i) == ' ')
            emptyInstructions = true;

    // The second error check looks for the appropriate number of characters inside the code
    if ((m_codeString.size()+1) % 7 != 0 || emptyInstructions)
    {
        // we exit if the code has unfinished instructions;
        code.push_back(0x00FE);
        return;
    }

    // We compute each instruction line by line
    for (int i = 0; i < m_codeString.size(); i++)
    {
        if (m_codeString.at(i) == 'x')
        {
            i++;
            code.push_back(HexStringToNumber(m_codeString, i));
        }
    }

    // We also add 2 hidden instructions at the end of the code, to make sure it stops running.
    // There are 2 of them in case the user ends the program with a skip instruction.
    code.push_back(0x00FE);
    code.push_back(0x00FE);
}

// This function updates the buttons if the simulation is not running
void CommandBox::UpdateButtons(sf::RenderWindow& window)
{
    if (!m_valmac->m_isRunning)
    {
        m_loadButton->Update(window);
        m_saveButton->Update(window);
        m_initButton->Update(window);
        m_runButton->Update(window);
    }
}

// This function renders a blue rectangle behind the line that is currently running
void CommandBox::RenderCurrentLine(sf::RenderWindow& window)
{
    sf::RectangleShape rectangle;
    int lineNumber = (m_valmac->PC - 0x200) / 2;

    rectangle.setSize(sf::Vector2f(188, 51));
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition(m_textPosition + sf::Vector2f(0,1 + lineNumber * DistanceBetweenRows));
    window.draw(rectangle);
}

// This function renders everything on the screen
void CommandBox::Render(sf::RenderWindow& window)
{
    RenderCode(window);
    RenderCodePointer(window);
    RenderHUD(window);
    RenderPCTimersI(window);
    RenderStack(window);
    RenderRegisters(window);
    RenderScreen(window);
    RenderKeypad(window);
}

// This function reads code from _code.txt
void CommandBox::LoadFromFile()
{
    std::ifstream file("_code.txt");
    m_codeString.clear();
    m_numberOfLines = 0;
    std::string line;
    while (file >> line)
    {
        m_codeString += line;
        m_codeString += '\n';
        m_numberOfLines++;
    }
    m_codeString.erase(m_codeString.size() - 1, 1);
    m_numberOfLines--;
    m_codeText.setString(m_codeString);
    file.close();
}

// This function saves the current code to _code.txt
void CommandBox::SaveToFile()
{
    std::ofstream file("_code.txt");
    file << m_codeString;
    file.close();
}

// This function initialises the emulator
void CommandBox::InitValmac()
{
    if (!m_valmac->m_isReady)
    {
        m_valmac->initialize();
    }
}

// This function runs the code if the emulator was intialised
void CommandBox::RunCode()
{
    if (m_valmac->m_isReady)
    {
        Compile(m_valmac->MasterMind);
        m_valmac->load_program(m_valmac->MasterMind);
    }
}

// This function moves the code pointer depending on the place the used clicked
void CommandBox::MoveCodePointer(sf::Vector2f mousePos)
{
    // If the user clicks outside of the code, we do nothing
    if (mousePos.x < 370 || mousePos.x > 490)
        return;

    // We calculate how many lines were scrolled, and what line the user has pressed
    int numberOfScrolls = (m_originalTextPosition.y - m_textPosition.y) / DistanceBetweenRows;
    int linePressed = numberOfScrolls + ceil((mousePos.y - m_originalTextPosition.y) / DistanceBetweenRows);

    if (linePressed >= 0)
    {
        m_codePointer = linePressed * 7;

        // We move the codePointer depending on the location on the OX axis
        if (mousePos.x < 400)
            m_codePointer -= 5;
        else if(mousePos.x < 430)
            m_codePointer -= 4;
        else if (mousePos.x < 460)
            m_codePointer -= 3;
        else
            m_codePointer -= 2;
        
        if (m_codePointer > m_codeString.size())
            m_codePointer = m_codeString.size();

        if (m_codePointer < 2)
            m_codePointer = 2;
    }
}
