#pragma once
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

class Valmac;
class LoadButton;
class SaveButton;
class InitButton;
class RunButton;

/*
	This class handles the interactions between the user and the emulator.
	It enables the user to input the code from the keyboard, and watch the 
	simulation run step by step.
	The code can also be read from or saved to a file. 
*/
class CommandBox
{
private:
	Valmac* m_valmac;

	sf::Font m_font;

	sf::Vector2f m_originalTextPosition;
	sf::Vector2f m_textPosition;

	sf::Text m_codeText;
	std::string m_codeString;

	int m_codePointer;
	int m_numberOfLines;
	sf::Text m_memoryIndicatorsText;
	std::string m_memoryIndicatorsString;

	sf::RectangleShape m_HUD;
	sf::Texture m_HUDTexture;

	std::shared_ptr<LoadButton> m_loadButton;
	std::shared_ptr<SaveButton> m_saveButton;
	std::shared_ptr<InitButton> m_initButton;
	std::shared_ptr<RunButton> m_runButton;
	
	void AddLine();
	void AddKey(const char key);
	void DeleteKey();
	void ClearCode();

	std::string NumberToHexString(int number);
	uint16_t HexStringToNumber(std::string& text, int pos);

	void RenderHUD(sf::RenderWindow& window);
	void RenderRegisters(sf::RenderWindow& window);
	void RenderCode(sf::RenderWindow& window);
	void RenderPCTimersI(sf::RenderWindow& window);
	void RenderStack(sf::RenderWindow& window);
	void RenderScreen(sf::RenderWindow& window);
	void RenderKeypad(sf::RenderWindow& window);
	void RenderCodePointer(sf::RenderWindow& window);

public:
	CommandBox(Valmac* valmac);

	void ComputeScroll(float scroll);
	void ComputeInput();

	void Compile(std::vector<uint16_t>& code);

	void UpdateButtons(sf::RenderWindow& window);

	void RenderCurrentLine(sf::RenderWindow& window);
	void Render(sf::RenderWindow& window);

	void LoadFromFile();
	void SaveToFile();
	void InitValmac();
	void RunCode();

	void MoveCodePointer(sf::Vector2f mousePos);
};

