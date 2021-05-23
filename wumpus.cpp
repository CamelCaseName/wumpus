#include "wumpus.h"
//#define DEBUG

//console size 80*24 chars
//create frame buffer to output directly to screen
// data structure -> 2d vector array (scalability after compile, so i can change world size dynamically)?, use bit encoding for cell state.. 
// or use a list of cell objects which can have multiple neighboors,so kinda like a graph but also bit encoding because efficient.

//cell state encoding:
//the attributes are just the states added up, so one bit is one state
//all possible states:
//	Agent	128
//	Breeze	1
//	Gold	8
//	Pit		16
//	Stench	32
//	Wumpus	64
//	Visited 512

/// <disclaimer1>
/// I know i mixed the windows proprietary console controls and the VT100 terminal control sequences
/// But i do't want to fix it, there is no need (this should be windows only, right?) 
/// </disclaimer1>


/// <disclaimer2>
/// i accidentally saved the file with unicode encoding, so now i have to replace all box drawing chars with the \x hex represenations. 
/// LOOKS SHIT, but is necessary
/// 
/// or i couold change the code page *shrug*
/// </disclaimer2>

short world_size = 8, x = 0, y = 0, old_x = 0, old_y = 0; //agent position variables
world map;
agent_local player;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO s;
COORD console_buffer_size;

//closes the console window when ctrl+c is called
BOOL WINAPI ConsoleHandler(DWORD ctrl_type) {
	if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
		ExitProcess(0);
	}
	return FALSE;
}

//initializes the map
void initialize_map() {
	//init world with correct size (minimum 4)
	if (world_size > 3) {
		map.set_size(world_size);
	}
	else {
		map.set_size(4);
	}

	//fill map randomly
	if (map.random_fill()) {
		//map filled ok
	}
	else {
		//map filled with an error, fix!
	}
}

//clears and prepares the console screen
//see https://docs.microsoft.com/en-us/windows/console/console-functions
void initialize_console() {
	//vars and flags and so on
	CONSOLE_CURSOR_INFO cursor_info;
	COORD tl = { 0,0 };
	DWORD dwFlags = CONSOLE_FULLSCREEN_MODE;
	DWORD dwMode;
	CONSOLE_FONT_INFOEX console_font{};

	//
	//console init itself
	//

	//console text code page 437 (Original IBM) for the box drawing chars, aka "extended ascii"
	SetConsoleCP(437); //see https://www.ascii-codes.com/ page 437
	SetConsoleOutputCP(437);

	//window title
	SetConsoleTitle(TEXT("WUMPUS by Leonhard Seidel, Nr. 5467428"));

	//fullscreen window, no scrollbars
	SetConsoleDisplayMode(console, dwFlags, &console_buffer_size);
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);

	//enable some console flags (vt100 sequence processing (mouse input should be disabled, but somehow isn't))
	GetConsoleMode(console, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console, ENABLE_EXTENDED_FLAGS | (dwMode & ~ENABLE_QUICK_EDIT_MODE));

	//clear screen once
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, chars = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacterW(console, ' ', chars, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, chars, tl, &written);

	//get console buffer and font and stuff
	GetCurrentConsoleFontEx(console, false, &console_font);
	console_buffer_size.X = GetSystemMetrics(SM_CXSCREEN) / GetConsoleFontSize(console, console_font.nFont).X;
	console_buffer_size.Y = GetSystemMetrics(SM_CYSCREEN) / GetConsoleFontSize(console, console_font.nFont).Y;
	SetConsoleCursorPosition(console, tl);
	printf("original buffer size : %d x %d, displaysize: %d x %d, ", console_buffer_size.X, console_buffer_size.Y, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	//adjust to 8x8 char size of a cell
	console_buffer_size.X = ((int)(console_buffer_size.X / 8)) * 8;
	console_buffer_size.Y = (((int)(console_buffer_size.Y / 8)) * 8) + 2;
	SetConsoleScreenBufferSize(console, console_buffer_size);
	printf("adjusted buffer size : %d x %d, fontsize = %d x %d", console_buffer_size.X, console_buffer_size.Y, GetConsoleFontSize(console, console_font.nFont).X, GetConsoleFontSize(console, console_font.nFont).Y);

	//display game title (now with colors)
	std::cout << "\x1b[2;0H";
	float frequency = .3f;
	short r, g, b;
	//printf("\x1b[38;2;255;100;0mTRUECOLOR\x1b[0m\n");
	for (short i = 0; i < (console_buffer_size.X - 39) / 2; i++) {
		r = (short)(sin(frequency * i + 0) * 127 + 128);
		g = (short)(sin(frequency * i + 2) * 127 + 128);
		b = (short)(sin(frequency * i + 4) * 127 + 128);
		std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcd";
	}
	r = 128;
	g = (short)(sin(2) * 127 + 128);
	b = (short)(sin(4) * 127 + 128);
	std::cout << "\xb9\x1b[46;30;7mWUMPUS by Leonhard Seidel, Nr. 5467428" << "\x1b[0m\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcc";
	for (short i = 0; i < (console_buffer_size.X - 39) / 2; i++) {
		r = (short)(sin(frequency * i + 0) * 127 + 128);
		g = (short)(sin(frequency * i + 2) * 127 + 128);
		b = (short)(sin(frequency * i + 4) * 127 + 128);
		std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcd";
	}
	std::cout << "\x1b[0m";

	//disable cursor
	GetConsoleCursorInfo(console, &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(console, &cursor_info);

}

//draws a single cell https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#designate-character-set
void draw_cell(short x, short y) {
	//some drawing magic (ciode looks shit, but works flawless)
	//if (x > 0) {
	//	x--;
	//}
	std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";
	if (!(map.get_cell(x, y) & VISITED)) {
		if (y == world_size - 1) {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb5  \xc6\xb1\xb1\xb1\xb1\xb1\xb1\x1b[16D\x1b[B";
		}

		std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
		std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
		if (x == 0) {
			std::cout << "\xb1\xb1            \xd0\xd0\x1b[16D\x1b[B";
			std::cout << "\xb1\xb1            \xd2\xd2\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xd0\xd0      ";
			if (x == world_size - 1) {
				std::cout << "      \xb1\xb1\x1b[16D\x1b[B";
				std::cout << "\xd2\xd2            \xb1\xb1\x1b[16D\x1b[B";
			}
			else {
				std::cout << "      \xd0\xd0\x1b[16D\x1b[B";
				std::cout << "\xd2\xd2            \xd2\xd2\x1b[16D\x1b[B";
			}
		}
		std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
		std::cout << "\xb1\xb1" << x << "," << y;
		for (short i = 0; i < 2 - (short)x / 10; i++) {
			std::cout << " ";
		}
		for (short i = 0; i < 2 - (short)y / 10; i++) {
			std::cout << " ";
		}
		std::cout << "     ""\xb1\xb1\x1b[16D\x1b[B";
		if (y == 0) {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
		}
		else {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb5  \xc6\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
		}
	}
	else {
		//if cell is at the top world border
		if (y == world_size - 1) {
			std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
		}

		//if cell has stench
		if (map.get_cell(x, y) & STENCH) {
			std::cout << "\xdb\xdb\x1b[33m\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
			std::cout << "\xdb\xdb\x1b[33m\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
			std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
		}

		//if cell is at the left world border
		if (x == 0) {
			std::cout << "\xdb\xdb            \xd0\xd0\x1b[16D\x1b[B";
			std::cout << "\xdb\xdb            \xd2\xd2\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xd0\xd0      ";
			//if cell is at the right world border
			if (x == world_size - 1) {
				std::cout << "      \xdb\xdb\x1b[16D\x1b[B";
				std::cout << "\xd2\xd2            \xdb\xdb\x1b[16D\x1b[B";
			}
			else {
				std::cout << "      \xd0\xd0\x1b[16D\x1b[B";
				std::cout << "\xd2\xd2            \xd2\xd2\x1b[16D\x1b[B";
			}
		}

		//if the cell has the gold
		if (map.get_cell(x,y) & GOLD) {
			std::cout << "\x1b[6C\x1b[A\x1b[33;1m\xf0\xdc\xdb\xf0\x1b[10D\x1b[B\x1b[37;22m";
		}

		//if the cell is breezy
		if (map.get_cell(x, y) & 3/*3 becuase it checks the first 2 bytes, and we can reach 3 breezes, but not 4*/) {
			if (map.get_cell(x, y) & AGENT) {

			}
			std::cout << "\xdb\xdb\x1b[36m\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
			std::cout << "\xdb\xdb" << x << "," << y << "\x1b[36m";
			//adjustement for 2 space wide numbers
			for (short i = 0; i < 2 - (short)x / 10; i++) {
				std::cout << "\xb1";
			}
			for (short i = 0; i < 2 - (short)y / 10; i++) {
				std::cout << "\xb1";
			}
			std::cout << "\xb1\xb1\xb1\xb1\xb1\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
			std::cout << "\xdb\xdb" << x << "," << y;
			//adjustement for 2 space wide numbers
			for (short i = 0; i < 2 - (short)x / 10; i++) {
				std::cout << " ";
			}
			for (short i = 0; i < 2 - (short)y / 10; i++) {
				std::cout << " ";
			}
			std::cout << "     ""\xdb\xdb\x1b[16D\x1b[B";
		}

		//if cell is at the lower world border
		if (y == 0) {
			std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
		}
		else {
			std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
		}

		//draw agent over cell, is easier then always checking in every line (saves ifs)
		if (map.get_cell(x,y) & AGENT) {
			std::cout << "\x1b[35m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
			if (x != old_x || y != old_y) {
				draw_cell(old_x, old_y);
			}
		}
	}
}

//puts the map in the console
void draw_map() {
	COORD pos = { 0,2 };
	SetConsoleCursorPosition(console, pos);
	for (short i = world_size - 1; i >= 0; --i) {
		for (short j = 0; j < world_size; j++) {
			//printf("%d,%d:%d ", i - 1, j, (int)map.get_cell(i - 1, j));
			draw_cell(j, i);
		}
	}
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
}

void redraw_map() {
	draw_cell(player.get_x_position(), player.get_y_position());
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
}

int main() {
	//init stuff
	initialize_map();
	initialize_console();
	draw_map();

	//main game loop
	while (1) {
		Sleep(33);
		player.walk(&x, &y, &old_x, &old_y, &world_size);
		if (x != old_x || y != old_y) {
			map.update(x, y, old_x, old_y);
			redraw_map();
			Sleep(250);

		}
	}
}
