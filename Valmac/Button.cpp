#include "Button.h"

// The constructor sets up the position of the button
Button::Button(CommandBox* commandBox, sf::Vector2f position, sf::Vector2f size)
{
	m_commandBox = commandBox;
	m_rectangle.setPosition(position);
	m_rectangle.setSize(size);
	m_rectangle.setFillColor(sf::Color(0,0,0,0));
}

// This function calls the Action() method when the button is pressed
void Button::Update(sf::RenderWindow& window)
{
	if (m_rectangle.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window))) 
		&& sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		Action();
	}
}
