#include "Valmac.h"

uint16_t Valmac::get_program_opcode()
{
	if((PC & 1))
	{
		std::cout << "Emulator stopped: PC is odd\n";
		m_isRunning = false;
	}

	if(PC > MEMORY_SIZE - sizeof(opcode))
	{
		std::cout << "Emulator stopped: PC is outside of memory\n";
		m_isRunning = false;
	}

	uint16_t l_opcode = memory[PC + 1] << 8 | memory[PC];

	// memory is unit8_t
	// memory[PC]

	return l_opcode;
}

inline void Valmac::step_PC()
{
	PC += 2; // PC cannot be odd

	if(PC > MEMORY_SIZE - sizeof(opcode))
		m_isRunning = false;
}

void Valmac::initialize()
{
	// Initialize registers and memory once
	PC = 0x200;  // Program counter starts at 0x200
	opcode = 0;      // Reset current opcode	
	I = 0;      // Reset index register
	SP = 0;      // Reset stack pointer

	// Clear display
	std::memset(gfx, 0, sizeof(gfx));

	// Clear stack
	std::memset(stack, 0, sizeof(stack));

	// Clear registers V0-VF
	std::memset(V, 0, sizeof(V));

	// Clear keypad
	std::memset(keypad, 0, sizeof(keypad));

	// Clear memory
	std::memset(memory, 0, MEMORY_SIZE);

	//// Load fontset
	load_fontSet();

	// Reset timers	
	delay_timer = 0;
	sound_timer = 0;

	m_isReady = true;
}

bool Valmac::load_program(std::vector<uint16_t> pProgram)
{
	// Read program from a file
	std::memcpy(memory + PC, &pProgram.front(), pProgram.size() * sizeof(uint16_t));
	//			destination, source,		   size
	m_isReady = false;
	m_isRunning = true;
	return true;
}

void Valmac::emulateCycle()
{
	bool validOpcode = false;
	// Fetch Opcode
	opcode = get_program_opcode();
	// Decode Opcode
		
	//std::cout << "0x" << std::hex << opcode << "\n";

	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode)
		{
		case 0x0000:
	// 0x0000	=>	no operation
			validOpcode = true;
			step_PC();
			break;
		case 0x00EE:
	// 0x00EE	=>	return call
			validOpcode = true;
			if (SP == 0)
			{
				std::cout << "Emulator stopped: Stack underflow\n";
				m_isRunning = false;
				break;
			}

			SP--;
			PC = stack[SP];
			stack[SP] = 0;
			step_PC();
			break;
		case 0X00E0:
	// 0x00E0	=> clear display
			validOpcode = true;
			std::memset(gfx, 0, sizeof(gfx));
			step_PC();
			break;
		case 0x00FE:
	// 0x00FE	=>	end program
			validOpcode = true;
			m_isRunning = false;
			break;
		}
		break;

	case 0x1000:
	// 0x1NNN	=>	jump to NNN
		validOpcode = true;
		PC = opcode & 0x0FFF;
		break;

	case 0x2000:
	// 0x2NNN	=>	call subroutine at NNN
		validOpcode = true;
		if (SP == 16)
		{
			std::cout << "Emulator stopped: Stack overflow\n";
			m_isRunning = false;
			break;
		}
		stack[SP] = PC;
		SP++;
		PC = opcode & 0x0FFF;
		break;

	case 0x3000:
	// 0x3XNN   =>   skip if V[X] == NN
		validOpcode = true;
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			step_PC();
		step_PC();
		break;

	case 0x4000:
	// 0x4XNN   =>   skip if V[X] != NN
		validOpcode = true;
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			step_PC();
		step_PC();
		break;

	case 0x5000:
	// 0x5XY0   =>   skip if V[X] == V[Y]
		validOpcode = true;
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			step_PC();
		step_PC();
		break;

	case 0x6000:
	// 0x6XNN	=> V[X] = NN
		validOpcode = true;
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		step_PC();
		break;

	case 0x7000:
	// 0x7XNN	=> V[X] += NN
		validOpcode = true;
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		step_PC();
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
	//0x8XY0	=> V[X] = V[Y]
			validOpcode = true;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;

		case 0x0001:
	//0x8XY1	=> V[X] = V[x] | V[Y]
			validOpcode = true;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;

		case 0x0002:
	//0x8XY2	=> V[X] = V[x] & V[Y]
			validOpcode = true;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;
		case 0x0003:
	//0x8XY3	=> V[X] = V[x] ^ V[Y]
			validOpcode = true;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;
		case 0x0004:
	//0x8XY4	=> V[X] += V[Y]   V[F] = 1 if there is a carry or V[F] = 0 if there isn't
			validOpcode = true;
			if (255 - V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
				V[0x0F] = 1;
			else
				V[0x0F] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;
		case 0x0005:
	//0x8XY5	=> V[X] -= V[Y]   V[F] = 0 if there is a borrow or V[F] = 1 if there isn't
			validOpcode = true;
			if (V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4])
				// V[X] >= V[Y]   => no borrow
				V[0x0F] = 1;
			else
				// V[X] < V[Y]   => borrow
				V[0x0F] = 0;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			step_PC();
			break;
		case 0x0006:
	//0x8X06	=> V[X] = V[X]>>1   bit 0 goes to V[F]
			validOpcode = true;
			V[0x0F] = V[(opcode & 0x0F00) >> 8] & 0x0001;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			step_PC();
			break;
		case 0x0007:
	//0x8XY7	=> V[X] = V[Y] - V[X]   V[F] = 0 if there is a borrow or V[F] = 1 if there isn't
			validOpcode = true;
			if (V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8])
				// V[Y] >= V[X]   => no borrow
				V[0x0F] = 1;
			else
				// V[Y] < V[X]   => borrow
				V[0x0F] = 0;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			step_PC();
			break;
		case 0x000E:
	//0x8X0E	=> V[X] = V[X] << 1   most significant bit goes to V[F]
			validOpcode = true;
			V[0x0F] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			step_PC();
			break;
		}
		break;

	case 0x9000:
	// 0x9XY0   =>   skip if V[X] != V[Y]
		validOpcode = true;
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			step_PC();
		step_PC();
		break;

	case 0xA000:
	// 0xANNN	=> I = NNN
		validOpcode = true;
		I = opcode & 0x0FFF;
		step_PC();
		break;

	case 0xB000:
	// 0xBNNN	=> PC = V[0] + NNN
		validOpcode = true;
		PC = V[0] + (opcode & 0x0FFF);
		break;

	case 0xC000:
	// 0xCXNN	=> V[X] = rand() & NN
		validOpcode = true;
		V[(opcode & 0x0F00) >> 8] = (rand()%255) & (opcode & 0x00FF);
		step_PC();
		break;

	case 0xD000:
	// 0xDXYN	=> draw sprite
		validOpcode = true;
		{
		int X = V[(opcode & 0x0F00) >> 8];
		int Y = V[(opcode & 0x00F0) >> 4];
		int K = ((Y * 64) + X) % (64*32 - 7);
		int N = (opcode & 0x000F) + 1;
		
		for (int i = 0; i < N; i++)
		{
			int line = memory[I + i];
			for (int j = 7; j >= 0; j--)
			{
				gfx[K + j] ^= (line % 2);
				line /= 2;
			}
			K += 64;
		}
		}
		step_PC();
		break;

	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E:
	// 0xEX9E	=>	skip next if keypad[X] == 1
			validOpcode = true;
			if (keypad[V[(opcode & 0x0F00) >> 8]])
				step_PC();
			step_PC();
			break;
		case 0x00A1:
	// 0xEXA1	=>	skip next if keypad[X] == 0
			validOpcode = true;
			if (!keypad[V[(opcode & 0x0F00) >> 8]])
				step_PC();
			step_PC();
			break;
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x0000:
	// 0xF000	=>	ping
			validOpcode = true;
			std::cout << '\7';
			step_PC();
			break;
		case 0x0007:
	// 0xFX07	=>	V[X] = delay timer
			validOpcode = true;
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			step_PC();
			break;
		case 0x000A:
	// 0xFX0A	=>	V[X] = get key
			validOpcode = true;
			while (!(keypad[0] || keypad[1] || keypad[2] || keypad[3] || keypad[4] || keypad[5] || keypad[6] || keypad[7]
				|| keypad[8] || keypad[9] || keypad[10] || keypad[11] || keypad[12] || keypad[13] || keypad[14] || keypad[15]))
				get_input();
			for (int8_t i = 0; i < 16; i++)
				if (keypad[i])
				{
					V[(opcode & 0x0F00) >> 8] = i;
					break;
				}
			step_PC();
			break;
		case 0x0015:
	// 0xFX15	=>	delay timer = V[X]
			validOpcode = true;
			delay_timer = V[(opcode & 0x0F00) >> 8];
			step_PC();
			break;
		case 0x0018:
	// 0xFX18	=>	sound timer = V[X]
			validOpcode = true;
			sound_timer = V[(opcode & 0x0F00) >> 8];
			step_PC();
			break;
		case 0x001E:
	// 0xFX1E	=>	I += V[X]
			validOpcode = true;
			I += V[(opcode & 0x0F00) >> 8];
			step_PC();
			break;
		case 0x0029:
	// 0xFX29	=>	I = V[X] sprite addr
			validOpcode = true;
			I = V[(opcode & 0x0F00) >> 8] * 5;
			step_PC();
			break;
		case 0x0033:
	// 0xFX33	=>	store V[X] BCD's digits in memory starting from I
			validOpcode = true;
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			step_PC();
			break;
		case 0x0055:
	// 0xFX55	=>	store V[0] to V[X] in memory starting from I
			validOpcode = true;
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
				memory[I + i] = V[i];
			step_PC();
			break;
		case 0x0065:
	// 0xFX65	=>	load V[0] to V[X] from memory starting from I
			validOpcode = true;
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
				V[i] = memory[I + i];
			step_PC();
			break;
		}
		break;
	}

	if (!validOpcode)
	{
		std::cout << "Emulator stopped: Unknown instruction\n";
		m_isRunning = false;
	}

	// Update timers
	if (delay_timer > 0)
	{
		delay_timer--;
	}

	if (sound_timer > 0)
	{
		std::cout << '\7';
		sound_timer--;
	}
}

// This function gets the input for the keypad during the simulation
void Valmac::get_input()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
	{
		keypad[0] = 1;
	}
	else
	{
		keypad[0] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		keypad[1] = 1;
	}
	else
	{
		keypad[1] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		keypad[2] = 1;
	}
	else
	{
		keypad[2] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		keypad[3] = 1;
	}
	else
	{
		keypad[3] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
	{
		keypad[4] = 1;
	}
	else
	{
		keypad[4] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
	{
		keypad[5] = 1;
	}
	else
	{
		keypad[5] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
	{
		keypad[6] = 1;
	}
	else
	{
		keypad[6] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
	{
		keypad[7] = 1;
	}
	else
	{
		keypad[7] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
	{
		keypad[8] = 1;
	}
	else
	{
		keypad[8] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
	{
		keypad[9] = 1;
	}
	else
	{
		keypad[9] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		keypad[10] = 1;
	}
	else
	{
		keypad[10] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
	{
		keypad[11] = 1;
	}
	else
	{
		keypad[11] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		keypad[12] = 1;
	}
	else
	{
		keypad[12] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		keypad[13] = 1;
	}
	else
	{
		keypad[13] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		keypad[14] = 1;
	}
	else
	{
		keypad[14] = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		keypad[15] = 1;
	}
	else
	{
		keypad[15] = 0;
	}
}

// This function loads the fontset at the beginnig of the memory, form address 0 up to 79
void Valmac::load_fontSet()
{
	//Load 0
	memory[0] = 0xF0;
	memory[1] = 0x90;
	memory[2] = 0x90;
	memory[3] = 0x90;
	memory[4] = 0xF0;

	//Load 1
	memory[5] = 0x20;
	memory[6] = 0x60;
	memory[7] = 0x20;
	memory[8] = 0x20;
	memory[9] = 0x70;

	//Load 2
	memory[10] = 0xF0;
	memory[11] = 0x10;
	memory[12] = 0xF0;
	memory[13] = 0x80;
	memory[14] = 0xF0;

	//Load 3
	memory[15] = 0xF0;
	memory[16] = 0x10;
	memory[17] = 0xF0;
	memory[18] = 0x10;
	memory[19] = 0xF0;

	//Load 4
	memory[20] = 0x90;
	memory[21] = 0x90;
	memory[22] = 0xF0;
	memory[23] = 0x10;
	memory[24] = 0x10;

	//Load 5
	memory[25] = 0xF0;
	memory[26] = 0x80;
	memory[27] = 0xF0;
	memory[28] = 0x10;
	memory[29] = 0xF0;

	//Load 6
	memory[30] = 0xF0;
	memory[31] = 0x80;
	memory[32] = 0xF0;
	memory[33] = 0x90;
	memory[34] = 0xF0;

	//Load 7
	memory[35] = 0xF0;
	memory[36] = 0x10;
	memory[37] = 0x20;
	memory[38] = 0x40;
	memory[39] = 0x40;

	//Load 8
	memory[40] = 0xF0;
	memory[41] = 0x90;
	memory[42] = 0xF0;
	memory[43] = 0x90;
	memory[44] = 0xF0;

	//Load 9
	memory[45] = 0xF0;
	memory[46] = 0x90;
	memory[47] = 0xF0;
	memory[48] = 0x10;
	memory[49] = 0xF0;

	//Load A
	memory[50] = 0xF0;
	memory[51] = 0x90;
	memory[52] = 0xF0;
	memory[53] = 0x90;
	memory[54] = 0x90;

	//Load B
	memory[55] = 0xE0;
	memory[56] = 0x90;
	memory[57] = 0xE0;
	memory[58] = 0x90;
	memory[59] = 0xE0;

	//Load C
	memory[60] = 0xF0;
	memory[61] = 0x80;
	memory[62] = 0x80;
	memory[63] = 0x80;
	memory[64] = 0xF0;

	//Load D
	memory[65] = 0xE0;
	memory[66] = 0x90;
	memory[67] = 0x90;
	memory[68] = 0x90;
	memory[69] = 0xE0;

	//Load E
	memory[70] = 0xF0;
	memory[71] = 0x80;
	memory[72] = 0xF0;
	memory[73] = 0x80;
	memory[74] = 0xF0;

	//Load F
	memory[75] = 0xF0;
	memory[75] = 0x80;
	memory[77] = 0xF0;
	memory[78] = 0x80;
	memory[79] = 0x80;
}
