// Valmac8.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Emulation execution is separate, mostly controller by PC.
//
// NB students may be more familiar with uint16_t than unsigned short
// similarly   may be more familiar with uint8_t  than unsigned char
//

#include <SFML/Graphics.hpp>
#include "Valmac.h"
#include "CommandBox.h"

int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Valmac : A0001151");
   
    Valmac myValmac;
    CommandBox commandBox(&myValmac);

    // While the emulation is not running, the delay is 0
    sf::Clock delayTimer;
    float delay = 0.0f;
    // Main loop
    while (window.isOpen())
    {
        // Handle any pending SFML events
        // These cover keyboard, mouse,joystick etc.
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseWheelScrolled:
                commandBox.ComputeScroll(event.mouseWheelScroll.delta);
                break;
            case sf::Event::KeyPressed:
                if (!myValmac.m_isRunning)
                    commandBox.ComputeInput();
                break;
            default:
                break;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            commandBox.MoveCodePointer(sf::Vector2f(sf::Mouse::getPosition(window)));

        commandBox.UpdateButtons(window);
        window.clear();

        // If the emulator is running, we get the input and render the current line, then emulate it
        if (myValmac.m_isRunning) 
        {
            delay = 0.5f;
            myValmac.get_input();
            commandBox.RenderCurrentLine(window);
            commandBox.Render(window);
            window.display();
            while (delayTimer.getElapsedTime().asSeconds() < delay/2);
            delayTimer.restart();

            window.clear();
            commandBox.RenderCurrentLine(window);
            myValmac.emulateCycle();
        }
        else
            delay = 0.0f;


        commandBox.Render(window);
        // Get the window to display its contents
        window.display();

        while (delayTimer.getElapsedTime().asSeconds() < delay/2);
        delayTimer.restart();
    }

    std::cout << "Good bye 8-bit world!\n";
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
