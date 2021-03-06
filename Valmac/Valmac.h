#pragma once
#include <cassert>
#include <cstring>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#define MEMORY_SIZE	(4096)

class Valmac
{
public:
	//The Valmac has around 35 opcodes which are all two bytes long. 
	uint16_t opcode;

	////4K memory in total
	// NB 1K is 1024 bytes (not 1000)
	uint8_t memory[4096];

	// CPU registers :  15, 8 - bit general purpose registers named V0, V1 up to VE.
	// The 16th register, Vf, is used  for the ?carry flag
	uint8_t  V[16];

	//There is an Index register I and a program counter (PC) which can have a value from 0x000 to 0xFFF
	uint16_t I;
	uint16_t PC;
	//The systems memory map :
	// 0x000 - 0x1FF - interpreter (contains font set in emu)
	// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
	// 0x200 - 0xFFF - Program ROM and work RAM

	//The graphics system : There is one instruction that draws sprite to the screen. Drawing is done in XOR mode 
	//and if a pixel is turned off as a result of drawing, the VF register is set. 
	//This is used for collision detection.
	//The graphics are black and white and the screen has a total of 2048 pixels(64 x 32).
	//This can easily be implemented using an array that hold the pixel state(1 or 0) :
	uint8_t gfx[64 * 32];

	//no Interupts or hardware registers.
	//but there are two timer registers that count at 60 Hz.
	//When set above zero they will count down to zero.
	uint8_t delay_timer;
	uint8_t sound_timer;

	/*
	It is important to know that the instruction set has opcodes that allow the program to jump
	to a certain address or call a subroutine. While the specification don?t mention a stack, you will need
	to implement one as part of the interpreter yourself. The stack is used to remember the current location
	before a jump is performed. So anytime you perform a jump or call a subroutine, store the program counter
	in the stack before proceeding. The system has 16 levels of stack and in order to remember which level of
	the stack is used, you need to implement a stack pointer (SP).
	*/
	uint16_t stack[16];
	uint16_t SP;

	//Finally, the HEX based keypad(0x0 - 0xF), you can use an array to store the 
	//current state of the key.
	uint8_t keypad[16];
	/*
		1	2	3	C
		4	5	6	D
		7	8	9	E
		A	0	B	F
	*/

	uint16_t get_program_opcode();
	inline void step_PC();

	bool m_isReady{ false };
	bool m_isRunning{ false };

	std::vector<uint16_t> MasterMind;
	
	void initialize();
	bool load_program(std::vector<uint16_t> pProgram);
	void emulateCycle();//run c clock tick

	void get_input();
	void load_fontSet();
};

