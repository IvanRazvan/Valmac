#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "CommandBox.h"

/*
	This is a pure virtual class that defines the fundamental behaviour of a button.
*/
class Button
{
protected:
	CommandBox* m_commandBox;
	sf::RectangleShape m_rectangle;
	virtual void Action() = 0;
public:
	Button(CommandBox* commandBox, sf::Vector2f position, sf::Vector2f size);
	void Update(sf::RenderWindow& window);
};

