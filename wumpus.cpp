#include "wumpus.h"
//#define DEBUG

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
/// I am transitioning to use as much vt100 as possible tho, because why not.
/// </disclaimer1>


/// <disclaimer2>
/// i accidentally saved the file with unicode encoding, so now i have to replace all box drawing chars with their \x hex represenations. 
/// LOOKS SHIT, but is necessary
///
/// you can find an "explanation" in the draw_cell() function
///
/// </disclaimer2>

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD console_buffer_size;
CONSOLE_CURSOR_INFO cursor_info;

//create image files if necessary
bool wumpus::initialize_files() {
	//filestream to test for one of the images
	std::fstream file_test("pit1.bmp", std::fstream::in | std::fstream::binary);
	if (file_test.good()) {
		file_test.close();
		return true;
	}
	else {
		try {
			//create streams to write the files out
			file_test.open("pit1.bmp", std::fstream::out | std::fstream::binary);
			//should write the pit1 stream to the file
			file_test.write(&pit1[0], static_cast<std::streamsize>(4758));
			file_test.close();

			//write second file
			file_test.open("pit2.bmp", std::fstream::out | std::fstream::binary);
			file_test.write(&pit2[0], static_cast<std::streamsize>(4054));
			file_test.close();

			//write third file
			file_test.open("gold1.bmp", std::fstream::out | std::fstream::binary);
			file_test.write(&gold1[0], static_cast<std::streamsize>(4758));
			file_test.close();

			//write fourth file
			file_test.open("gold2.bmp", std::fstream::out | std::fstream::binary);
			file_test.write(&gold2[0], static_cast<std::streamsize>(4758));
			file_test.close();

			//write fifth file
			file_test.open("wumpus1.bmp", std::fstream::out | std::fstream::binary);
			file_test.write(&wumpus1[0], static_cast<std::streamsize>(4086));
			file_test.close();

			//write sixth file
			file_test.open("wumpus2.bmp", std::fstream::out | std::fstream::binary);
			file_test.write(&wumpus2[0], static_cast<std::streamsize>(4086));
			file_test.close();
			//return true to say "ok, we did it!!!!"
			return true;
		}
		catch (std::exception e) {
			return false;
		}
	}
	return false;
}

//initializes the map
void wumpus::initialize_map() {
	//init world with correct size (minimum 4)
	if (wumpus::world_size > 3) {
		map.set_size(wumpus::world_size);
	}
	else {
		map.set_size(4);
	}


	//fill map randomly
	if (map.random_fill()) { //change to random fill
		//map filled ok
	}
	else {
		ExitProcess(1);
	}
}

// error message thingy kindly borroughed from microsoft to debug with. f*ck me this windows debugging is ass. compiles on my destkop, not on my laptop (works on both)
// FUN FACT: calling this error message messager can produdce an error...
// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
void wumpus::ErrorExit(LPTSTR lpszFunction) {
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf = nullptr;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process
#pragma warning(push) 
#pragma warning(disable : 28183)
#pragma warning(push) 
#pragma warning(disable : 6067)
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
#pragma warning(pop)
#pragma warning(pop)
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

//clears and prepares the console screen, see https://docs.microsoft.com/en-us/windows/console/console-functions
void wumpus::initialize_console() {
	//vars and flags and so on
	CONSOLE_SCREEN_BUFFER_INFO s;
	DWORD dwFlags = CONSOLE_FULLSCREEN_MODE;
	DWORD dwMode;
	CONSOLE_FONT_INFOEX console_font{};

	//
	//console init itself
	//

	//console text code page 437 (Original IBM) for the box drawing chars, aka "extended ascii"
	SetConsoleCP(437); //see https://www.ascii-codes.com/ page 437
	SetConsoleOutputCP(437);

	//fullscreen window, no scrollbars
	//this throws an error if the function is not supported. tho i cannot find out whyyyy
	if (!SetConsoleDisplayMode(console, dwFlags, &console_buffer_size)) {
		//ErrorExit(const_cast<LPTSTR>("SetConsoleDisplayMode"));
	}
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);

	//enable some console flags (vt100 sequence processing (mouse input should be disabled, but somehow isn't))
	GetConsoleMode(console, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console, dwMode);

	//window title
	std::cout << "\x1b]2;WUMPUS by Leonhard Seidel, Nr. 5467428\x07";

	//get console buffer and font and stuff
	GetCurrentConsoleFontEx(console, false, &console_font);
	//console_buffer_size.X = GetSystemMetrics(SM_CXSCREEN) / GetConsoleFontSize(console, console_font.nFont).X - 4;
	//console_buffer_size.Y = GetSystemMetrics(SM_CYSCREEN) / GetConsoleFontSize(console, console_font.nFont).Y;
	GetConsoleScreenBufferInfo(console, &s);
	console_buffer_size = s.dwMaximumWindowSize;

	//clear screen once
	std::cout << "\x1b[0;0H";
	for (uint8_t i = 0; i <= console_buffer_size.Y; i++) {
		std::cout << "\x1b[2K\x1b[B";
	}
	std::cout << "\x1b[0;0H";

	//printf("original buffer size : %d x %d, displaysize: %d x %d, ", console_buffer_size.X, console_buffer_size.Y, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	//adjust to 8x8 char size of a cell
	if ((world_size * 8) + 2 < console_buffer_size.Y) {
		console_buffer_size.X = world_size * 16;
		console_buffer_size.Y = (world_size * 8) + 2;
		SetConsoleScreenBufferSize(console, console_buffer_size);
	}
	else {
		small_screen = true;
	}
	//printf("adjusted buffer size : %d x %d, fontsize = %d x %d", console_buffer_size.X, console_buffer_size.Y, GetConsoleFontSize(console, console_font.nFont).X, GetConsoleFontSize(console, console_font.nFont).Y);

	//set scrolling margins
	//std::cout << "\x1b[3;" << console_buffer_size.Y << "r";

	//display game title (now with colors)
	std::cout << "\x1b[1;0H";
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
	std::cout << "\x1b[?12l\x1b[?25l";
}

//clears the whole screen
void wumpus::clear_screen() {
	//fill all cells with emptyness
	std::cout << "\x1b[2;0H";
	for (uint8_t i = 2; i < console_buffer_size.Y + 2; i++) {
		std::cout << "\x1b[2K\x1b[B";
	}
}

//puts a message in the middle of the screen
void wumpus::draw_message(const char* message) {
	std::cout << "\x1b[""7\x1b[2;" << (console_buffer_size.X - strlen(message)) / 2 << "H\x1b[30;1;47m" << message << "\x1b[0m\x1b""8";
}

//draws the pit
void wumpus::pit_ending_draw(short x, short y) {
	//move cursor back to cell beginning
	if (small_screen) {
		std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H";
	}
	else {
		std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";
	}

	//if cell is at the top world border
	if (y == world_size - 1) {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
	}
	else {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
	}

	std::cout << "\xdb\xdb\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";
	std::cout << "\xdb\xdb\xb2\xb2\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";


	//if cell is at the left world border
	if (x == 0) {
		std::cout << "\xdb\xdb\xb2\xb2\xb1\xb1\xb0\xb0\xb0\xb0\xb1\xb1\xb2\xb2\xd0\xd0\x1b[16D\x1b[B";
		std::cout << "\xdb\xdb\xb2\xb2\xb1\xb1\xb0\xb0\xb0\xb0\xb1\xb1\xb2\xb2\xd2\xd2\x1b[16D\x1b[B";
	}
	else {
		std::cout << "\xd0\xd0\xb2\xb2\xb1\xb1\xb0\xb0";
		//if cell is at the right world border
		if (x == world_size - 1) {
			std::cout << "\xb0\xb0\xb1\xb1\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";
			std::cout << "\xd2\xd2\xb2\xb2\xb1\xb1\xb0\xb0\xb0\xb0\xb1\xb1\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";
		}
		else {
			std::cout << "\xb0\xb0\xb1\xb1\xb2\xb2\xd0\xd0\x1b[16D\x1b[B";
			std::cout << "\xd2\xd2\xb2\xb2\xb1\xb1\xb0\xb0\xb0\xb0\xb1\xb1\xb2\xb2\xd2\xd2\x1b[16D\x1b[B";
		}
	}



	std::cout << "\xdb\xdb\xb2\xb2\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";
	std::cout << "\xdb\xdb" << player.get_x_position() << "," << player.get_y_position();
	//adjustement for 2 space wide numbers
	for (short i = 0; i < 2 - (short)x / 10; i++) {
		std::cout << "\xb2";
	}
	for (short i = 0; i < 2 - (short)y / 10; i++) {
		std::cout << "\xb2";
	}
	std::cout << "\xb2\xb2\xb2\xb2\xb2\xdb\xdb\x1b[16D\x1b[B";


	//if cell is at the lower world border
	if (y == 0) {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
	}
	else {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
	}
}

void wumpus::wumpus_ending_draw(short x, short y) {
	//move cursor back to cell beginning
	if (small_screen) {
		std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H";
	}
	else {
		std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";
	}

	//if cell is at the top world border
	if (y == world_size - 1) {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
	}
	else {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
	}

	std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
	std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";


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
			std::cout << "              \xdb\xdb\x1b[16D\x1b[B";
		}
		else {
			std::cout << "      \xd0\xd0\x1b[16D\x1b[B";
			std::cout << "              \xd2\xd2\x1b[16D\x1b[B";
		}
	}



	std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
	std::cout << "\xdb\xdb" << player.get_x_position() << "," << player.get_y_position();
	//adjustement for 2 space wide numbers
	for (short i = 0; i < 2 - (short)x / 10; i++) {
		std::cout << " ";
	}
	for (short i = 0; i < 2 - (short)y / 10; i++) {
		std::cout << " ";
	}
	std::cout << "     \xdb\xdb\x1b[16D\x1b[B";


	//if cell is at the lower world border
	if (y == 0) {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
	}
	else {
		std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
	}
}

//draws a square of size size at the current cursor position
void wumpus::draw_block(double* size) {
	//round double
	if (*size == 0.5) {
		std::cout << "\xdb";
	}
	else {
		*size = (double)(int)(*size + 0.5);

		for (short k = 0; k < *size; k++) {
			for (short i = 0; i < *size; i++) {
				std::cout << "\xdb\xdb";
			}
			std::cout << "\x1b[" << (*size * 2) << "D\x1b[B";
		}
		std::cout << "\x1b[" << *size << "C\x1b[" << *size << "A";
	}
}

//draws from a bmp file to the console
void wumpus::draw_from_bmp(char* path) {

	//vars and the BMP structure
	try {
		BMP in(path);
		uint8_t channels = in.bmp_info_header.bit_count / 8;
		double square_size = 0;

		clear_screen();

		//change size depending on picture resolution
		if (console_buffer_size.X > 110) {
			square_size = (short)(console_buffer_size.X / (in.bmp_info_header.width));
			if (square_size > 2) {
				square_size = 2;
			}
		}
		else {
			square_size = .5;
		}
		std::cout << "\x1b[2;1H";

		//iterate through pixels and display them
		for (uint8_t y = 0; y < in.bmp_info_header.height; y++) {
			for (uint8_t x = 0; x < in.bmp_info_header.width; x++) {
				if (in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 2] < 20 && in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 1] < 20 && in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 0] < 20) {

				}
				else {
					std::cout << "\x1b[38;2;" << (unsigned short)in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 2];
					std::cout << ";" << (unsigned short)in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 1];
					std::cout << ";" << (unsigned short)in.data[channels * ((in.bmp_info_header.height - y - 1) * in.bmp_info_header.width + x) + 0] << "m";
					if (square_size > 0.5) {
						std::cout << "\x1b[" << y * (int)(square_size + .5) + 3 << ";" << x * (int)(square_size + 0.5) * 2 << "H";
					}
					else {
						std::cout << "\x1b[" << y * (int)(square_size + .5) + 3 << ";" << x * (int)(square_size + 0.5) << "H";
					}
					draw_block(&square_size);
				}
			}
		}

		//restore defaults
		std::cout << "\x1b[0m";
	}
	catch (std::runtime_error e) {
		std::cout << "\x1b[2;0H\x1b[40;22mCAN'T FIND IMAGE: " << path;
		Sleep(3000);
		ExitProcess(1);
	}

}

//draws the game over screen
void wumpus::draw_gameover() {
	//if we go into the menu
	if (esc_menu_request) {
		//switch to a new screen buffer and disable the cursor
		std::cout << "\x1b[?1049h\x1b[1;0H\x1b[0m\x1b[?12l\x1b[?25l";

		//cool new menu banner
		float frequency = .3f;
		short r, g, b;
		for (short i = 0; i < (console_buffer_size.X - 9) / 2; i++) {
			r = (short)(sin(frequency * i + 0) * 127 + 128);
			g = (short)(sin(frequency * i + 2) * 127 + 128);
			b = (short)(sin(frequency * i + 4) * 127 + 128);
			std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcd";
		}
		r = 128;
		g = (short)(sin(2) * 127 + 128);
		b = (short)(sin(4) * 127 + 128);
		std::cout << "\xb9\x1b[46;30;7mSettings" << "\x1b[0m\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcc";
		for (short i = 0; i < (console_buffer_size.X - 9) / 2; i++) {
			r = (short)(sin(frequency * i + 0) * 127 + 128);
			g = (short)(sin(frequency * i + 2) * 127 + 128);
			b = (short)(sin(frequency * i + 4) * 127 + 128);
			std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m\xcd";
		}
		std::cout << "\x1b[0m";

		Sleep(1000);

		std::cout << "\x1b[?1049l";

	}
	else {
		//move cursor to lowest point and reset color
		std::cout << "\x1b[2;0H\x1b[0;22m";

		clear_screen();

		//display a end screen when falling into a pit
		if (fell_in_pit) {
			//square_size = (short)(console_buffer_size.X / (13 * 4)); //string is 13 letters long
			//std::cout << "\x1b[4;2H\x1b[31m";
			//draw_block(&square_size);

			//choose one of the multiple end screens
			if (rand() % 2 == 0) {
				draw_from_bmp((char*)"pit1.bmp");
			}
			else {
				draw_from_bmp((char*)"pit2.bmp");
			}
		}
		else if (got_gold) {
			//choose one of the multiple end screens
			if (rand() % 2 == 0) {
				draw_from_bmp((char*)"gold1.bmp");
			}
			else {
				draw_from_bmp((char*)"gold2.bmp");
			}
		}
		else if (met_wumpus) {
			//choose one of the multiple end screens
			if (rand() % 100 == 0) {
				draw_from_bmp((char*)"wumpus1.bmp");
			}
			else {
				draw_from_bmp((char*)"wumpus2.bmp");
			}
		}

		//some waiting time
		Sleep(1000);

		//draw keybinds for exiting/restarting
		draw_message("PRESS ESC TO END THE GAME, PRESS RETURN TO RESTART");
	}

}

//draws a single cell 
void wumpus::draw_cell(short x, short y) {

	/*
		"Explanation"

		for most of the console formatting i use vt100 terminal sequences. these have been introduced to windows in ~2016,
		and are best practice by now. (better cross compatibility and so on)
		see: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

		there are two types of sequences: control sequence induces (CSI) sequences, and operating system command (OSC) sequences.
		no spaces are to be included in these sequences.
		CSI sequences start with the ESC char, followed by an '['
		the OSC sequences also begin with an ESC, but are followed by an ']'
		after the brackets, numbers can be used to set up a command. the numbers are seperated with a semicolon ';'
		at the end of a terminal sequence follows the char corresponding to the command.
		i put a table here with the ones i used. you can read more on these following the link above.

		### general info on the commands ###
		\x1b = ESC in hexadecimal notation
		\x1b[ = the beginning of a command

		### cursor commands ###
		\x1b[<n>A = move cursor up (n times)
		\x1b[<n>B = move cursor down (n times)
		\x1b[<n>C = move cursor right/forward (n times)
		\x1b[<n>D = move cursor left/backward (n times)
		\x1b""7 = store current cursor position {because of how the numbers after \x are interpreted, we have to seperate the ESC and the number/letter with a ""}
		\x1b""8 = restore stored cursor postition {same here...}
		\x1b[<y>;<n=x>H = set cursor to the given coord, starts on 1
		\x1b[?12l = disable cursor blinking {lowercase L at the end}
		\x1b[?25l = hide cursor {lowercase L at the end}

		### text color commands ###
		# all color commands apply to characters only, which are output after the color has been set.
		\x1b[0m = all default colors
		\x1b[<n>m = set text color, where n is a special number
		\x1b[38:<r>;<g>;<b>m = sets the foreground color to the rgb value given
		\x1b[48:<r>;<g>;<b>m = sets the background color to the rgb value given
		# for all other color values please refer to the section in the manual
		# https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting

		### miscellaneous commands ###
		\x1b[<n>K = clear line <n>
		\x1b]<string>\x07 = set window title, delimiter is the bell character.
		\x1b[<t>;<b>r = set screen scroll region from t to b (inclusive), son"t think i use this anymore
		\x1b[?1049h = switch to a new screen buffer (used for the menu)
		\x1b[?1049l = switch back to the main buffer

		### characters in the codepage 437 i used:
		\x01   ☺
		\xb0     ░
		\xb1   ▒
		\xb2     ▓
		\xb5   ╡
		\xb9     ╣
		\xba   ╔
		\xbb     ╗
		\xbf   ┐
		\xc0     └
		\xc6   ╞
		\xca     ╩
		\xcc   ╠
		\xcd     ═
		\xd0   ╨
		\xd2     ╥
		\xd7   ╫
		\xd9     ┘
		\xda   ┌
		\xdb     █
		\xdc   ▄
		\xf0     ≡
	*/

	//some drawing magic (code looks shit, but works flawless)
	std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

	//if cell is not visited, i.e. not discovered
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
		std::cout << "     \xb1\xb1\x1b[16D\x1b[B";
		if (y == 0) {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
		}
		else {
			std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb5  \xc6\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
		}
	}
	else {
		//if the cell is a pit
		if (map.get_cell(x, y) & PIT) {
			//set end screen var plus disable movement
			fell_in_pit = 1;
			if (playertype) {
				player.disable_walking();
			}
			else {
				mr_robot.disable_walking();
			}

			clear_screen();

			//move cursor back to cell beginning
			std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

			pit_ending_draw(x, y);

			//draw agent over cell, is easier then always checking in every line (saves ifs)
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;0;128;0m\x1b[2B\x1b[8D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
			}
			else {
				std::cout << "\x1b[38;2;0;128;0m\x1b[2B\x1b[8D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			pit_ending_draw(x, y);
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;0;64;0m\x1b[3B\x1b[8D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[37m\x1b[6A\x1b[2C";
			}
			else {
				std::cout << "\x1b[38;2;0;64;0m\x1b[3B\x1b[8D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[37m\x1b[6A\x1b[2C";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			pit_ending_draw(x, y);
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;;32;0m\x1b[4B\x1b[8D\x01\x1b[37m\x1b[6A\x1b[2C";
			}
			else {
				std::cout << "\x1b[38;2;0;32;0m\x1b[4B\x1b[8D\x01\x1b[37m\x1b[6A\x1b[2C";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			pit_ending_draw(x, y);

			//give player some time to think about their actions
			Sleep(500);

			//reset background color
			std::cout << "\x1b[40;22m";

			gameover = true;
		}

		//if the cell has the WUMPUS in it...
		else if (map.get_cell(x, y) & WUMPUS) {
			//set end screen var plus disable movement
			met_wumpus = true;
			if (playertype) {
				player.disable_walking();
			}
			else {
				mr_robot.disable_walking();
			};

			//reset color attributes
			std::cout << "\x1b[0m";

			clear_screen();

			//move cursor back to cell beginning
			std::cout << "\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

			wumpus_ending_draw(x, y);

			//draw agent over cell, is easier then always checking in every line (saves ifs)
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;128;;0m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb";
			}
			else {
				std::cout << "\x1b[38;2;128;0;0m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;64;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb";
			}
			else {
				std::cout << "\x1b[38;2;64;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			if (player_walk_animation) {
				std::cout << "\x1b[38;2;32;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
			}
			else {
				std::cout << "\x1b[38;2;32;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
			}

			//delay next frame
			Sleep(300);

			//draw next frame
			wumpus_ending_draw(x, y);

			//give player some time to think about their actions
			Sleep(500);

			//reset background color
			std::cout << "\x1b[40;22m";

			//game ends
			gameover = true;
		}

		//"normal" cell
		else {
			//set background color
			std::cout << "\x1b[48;2;30;30;30m";

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
			if (map.get_cell(x, y) & GOLD) {
				std::cout << "\x1b[6C\x1b[2A\x1b[33;1m\x1b[48;2;81;77;47m    \x1b[B\x1b[4D\xf0\xdc\xdb\xf0\x1b[10D\x1b[B\x1b[37;22m\x1b[48;2;30;30;30m";
			}

			//if the cell is breezy
			if (map.get_cell(x, y) & 3/*3 becuase it checks the first 2 bytes, and we can reach 3 breezes, but not 4*/) {
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
				std::cout << "     \xdb\xdb\x1b[16D\x1b[B";
			}

			//if cell is at the lower world border
			if (y == 0) {
				std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
			}
			else {
				std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
			}

			//draw agent over cell, is easier then always checking in every line (saves ifs)
			if (map.get_cell(x, y) & AGENT) {
				if (player_walk_animation) {
					player_walk_animation = 0;
					std::cout << "\x1b[32m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
					if (x != old_x || y != old_y) {
						draw_cell(old_x, old_y);
					}
				}
				else {
					player_walk_animation = 1;
					std::cout << "\x1b[32m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
					if (x != old_x || y != old_y) {
						draw_cell(old_x, old_y);
					}
				}
			}

			if (map.get_cell(x, y) & GOLD) {
				//set end screen var plus disable movement
				got_gold = 1;
				if (playertype) {
					player.disable_walking();
				}
				else {
					mr_robot.disable_walking();
				}

				//left = 1, top = 2, right = 4, bottom = 8
				uint8_t border = 0, i_at_border_hit = 0;

				Sleep(500);

				//set colors plus move cursor to the correct position
				std::cout << "\x1b[33;1m\x1b[43;1m\x1b[" << ((world_size - y) * 8) - 5 << ";" << x * 16 + 1 << "H\x1b[3B\x1b[6C";
				/*
				for (uint8_t i = 1; i < world_size * 8 + 6; i += 2) { //takes 4 + 1 cylces (<6) to fill the cell with the gold
					//after 4 cycles, cell is filled. plus 8 more, we fill the next

					//some really fucked up shit that just miraculously works (sometimes) ¯\_(ツ)_/¯ v0.1
					if (i == (x + 1) * 8 + 7) {
						i_at_border_hit = i;
						border += 1;
					}
					if (i == (world_size - y) * 8 + 7) {
						i_at_border_hit = i;
						border += 2;
					}
					if (i == (world_size - x + 1) * 8 + 7) {
						i_at_border_hit = i;
						border += 4;
					}
					if (i == (y + 2) * 8 + 7) {
						i_at_border_hit = i;
						border += 8;
					}

					std::cout << "\x1b[0m\x1b""7\x1b[1;112H| current borders: " << (int)border << "\x1b""8\x1b[33;1m\x1b[43;1m";

					//right --> left
					if (!(border & 2)) {
						if (i == 1) {
							//fill central square
							std::cout << "\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D";
							std::cout << "\x1b[B\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D\x1b[2A";
						}
						else {
							std::cout << "\x1b[A\x1b[2D";
						}
						std::cout << "\xdb\xdb\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\xdb\xdb";
						}
					}
					else {
						std::cout << "\x1b[4C";
						for (uint8_t k = 1; k < i_at_border_hit + (i - i_at_border_hit) / 2; k++) {
							std::cout << "\x1b[2C";
						}
					}

					//top --> bottom
					if (!(border & 4)) {
						std::cout << "\x1b[B\x1b[2D\xdb\xdb";
						std::cout << "\x1b[B\x1b[2D\xdb\xdb";
						if (!(border & 2)) {
							for (uint8_t k = 0; k < i; k++) {
								std::cout << "\x1b[B\x1b[2D\xdb\xdb";
							}
						}
						else {
							for (uint8_t k = 1; k < i_at_border_hit + (i - i_at_border_hit) / 2; k++) {
								std::cout << "\x1b[B\x1b[2D\xdb\xdb";
							}
						}

					}
					else {
						//std::cout << "\x1b[2B";
						for (uint8_t k = 1; k < i_at_border_hit + (i - i_at_border_hit) / 2; k++) {
							std::cout << "\x1b[B";
						}
					}

					//left --> right
					if (!(border & 8)) {
						std::cout << "\x1b[4D\xdb\xdb";
						std::cout << "\x1b[4D\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\x1b[4D\xdb\xdb";
						}
					}
					else {
						std::cout << "\x1b[4D";
						for (uint8_t k = 1; k < i_at_border_hit + (i - i_at_border_hit) / 2; k++) {
							std::cout << "\x1b[2D";
						}
					}

					//bottom --> top
					if (!(border & 1)) {
						std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						}
					}
					else {
						std::cout << "\x1b[2A";
						for (uint8_t k = 1; k < i_at_border_hit + (i - i_at_border_hit) / 2; k++) {
							std::cout << "\x1b[A";
						}
					}
					Sleep(500);
				}*/
				for (uint8_t i = 1; i < 6; i += 2) { //takes 4 + 1 cylces (<6) to fill the cell with the gold

					//some really fucked up shit that just miraculously works (sometimes) ¯\_(ツ)_/¯ v0.5
					//right --> left
					if (i == 1) {
						//fill central square
						std::cout << "\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D";
						std::cout << "\x1b[B\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D\x1b[2A";
					}
					else {
						std::cout << "\x1b[A\x1b[2D";
					}
					std::cout << "\xdb\xdb\xdb\xdb";
					for (uint8_t k = 0; k < i; k++) {
						std::cout << "\xdb\xdb";
					}

					//top --> bottom
					std::cout << "\x1b[B\x1b[2D\xdb\xdb";
					std::cout << "\x1b[B\x1b[2D\xdb\xdb";
					for (uint8_t k = 0; k < i; k++) {
						std::cout << "\x1b[B\x1b[2D\xdb\xdb";
					}

					//left --> right
					std::cout << "\x1b[4D\xdb\xdb";
					std::cout << "\x1b[4D\xdb\xdb";
					for (uint8_t k = 0; k < i; k++) {
						std::cout << "\x1b[4D\xdb\xdb";
					}

					//bottom --> top
					std::cout << "\x1b[A\x1b[2D\xdb\xdb";
					std::cout << "\x1b[A\x1b[2D\xdb\xdb";
					for (uint8_t k = 0; k < i; k++) {
						std::cout << "\x1b[A\x1b[2D\xdb\xdb";
					}

					Sleep(200 - i * 15);
				}

				double eight = 8;
				for (short i = world_size - 1; i >= 0; --i) {
					for (short j = 0; j < world_size; j++) {
						std::cout << "\x1b[" << ((world_size - i) * 8) - 5 << ";" << j * 16 + 1 << "H";
						draw_block(&eight);
					}
				}

				Sleep(300);

				//vars for game end
				got_gold = true;
				gameover = true;
			}

			//reset background color + scrollbar
			ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
			std::cout << "\x1b[0m";
		}
	}
}

//draws the cells for the smaller map
void wumpus::draw_cell_small(short x, short y, short map_x, short map_y) {

	//edge case problem detection and mitigation
	if (x * 16 + 1 != 49) {

		//some drawing magic (code looks shit, but works flawless)
		std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

		//if cell is not visited, i.e. not discovered
		if (!(map.get_cell(map_x, map_y) & VISITED)) {
			if (map_y == world_size - 1) {
				std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[16D\x1b[B";
			}
			else {
				std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb5  \xc6\xb1\xb1\xb1\xb1\xb1\xb1\x1b[16D\x1b[B";
			}

			std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
			std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
			if (map_x == 0) {
				std::cout << "\xb1\xb1            \xd0\xd0\x1b[16D\x1b[B";
				std::cout << "\xb1\xb1            \xd2\xd2\x1b[16D\x1b[B";
			}
			else {
				std::cout << "\xd0\xd0      ";
				if (map_x == world_size - 1) {
					std::cout << "      \xb1\xb1\x1b[16D\x1b[B";
					std::cout << "\xd2\xd2            \xb1\xb1\x1b[16D\x1b[B";
				}
				else {
					std::cout << "      \xd0\xd0\x1b[16D\x1b[B";
					std::cout << "\xd2\xd2            \xd2\xd2\x1b[16D\x1b[B";
				}
			}
			std::cout << "\xb1\xb1            \xb1\xb1\x1b[16D\x1b[B";
			std::cout << "\xb1\xb1" << map_x << "," << map_y;
			for (short i = 0; i < (1 - (short)(map_x / 10)) + (map_x >= 0); i++) {
				std::cout << " ";
			}
			for (short i = 0; i < (1 - (short)(map_y / 10)) + (map_y >= 0); i++) {
				std::cout << " ";
			}
			std::cout << "     \xb1\xb1\x1b[16D\x1b[B";
			if (map_y == 0) {
				std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
			}
			else {
				std::cout << "\xb1\xb1\xb1\xb1\xb1\xb1\xb5  \xc6\xb1\xb1\xb1\xb1\xb1\xb1\x1b[7A";
			}
		}
		else {
			//if the cell is a pit
			if (map.get_cell(map_x, map_y) & PIT) {
				//set end screen var plus disable movement
				fell_in_pit = 1;
				if (playertype) {
					player.disable_walking();
				}
				else {
					mr_robot.disable_walking();
				}

				//move cursor to lowest point
				std::cout << "\x1b[2;0H\x1b[40;22m";

				clear_screen();

				//move cursor back to cell beginning
				std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

				pit_ending_draw(x, y);

				//draw agent over cell, is easier then always checking in every line (saves ifs)
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;0;128;0m\x1b[2B\x1b[8D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
				}
				else {
					std::cout << "\x1b[38;2;0;128;0m\x1b[2B\x1b[8D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				pit_ending_draw(x, y);
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;0;64;0m\x1b[3B\x1b[8D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[37m\x1b[6A\x1b[2C";
				}
				else {
					std::cout << "\x1b[38;2;0;64;0m\x1b[3B\x1b[8D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[37m\x1b[6A\x1b[2C";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				pit_ending_draw(x, y);
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;0;32;0m\x1b[4B\x1b[8D\x01\x1b[37m\x1b[6A\x1b[2C";
				}
				else {
					std::cout << "\x1b[38;2;0;32;0m\x1b[4B\x1b[8D\x01\x1b[37m\x1b[6A\x1b[2C";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				pit_ending_draw(x, y);

				//give player some time to think about their actions
				Sleep(500);

				//reset background color
				std::cout << "\x1b[40;22m";

				//game ends
				gameover = 1;
			}

			//if the cell has the WUMPUS in it...
			else if (map.get_cell(map_x, map_y) & WUMPUS) {
				//set end screen var plus disable movement
				met_wumpus = true;
				if (playertype) {
					player.disable_walking();
				}
				else {
					mr_robot.disable_walking();
				};

				//reset color attributes
				std::cout << "\x1b[0m";

				clear_screen();

				//move cursor back to cell beginning
				std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H";

				wumpus_ending_draw(x, y);

				//draw agent over cell, is easier then always checking in every line (saves ifs)
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;128;;0m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb";
				}
				else {
					std::cout << "\x1b[38;2;128;0;0m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;64;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb";
				}
				else {
					std::cout << "\x1b[38;2;64;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				if (player_walk_animation) {
					std::cout << "\x1b[38;2;32;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
				}
				else {
					std::cout << "\x1b[38;2;32;0;0m\x1b[2A\x1b[2D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
				}

				//delay next frame
				Sleep(300);

				//draw next frame
				wumpus_ending_draw(x, y);

				//give player some time to think about their actions
				Sleep(500);

				//reset background color
				std::cout << "\x1b[40;22m";

				//game ends
				gameover = true;
			}

			//"normal" cell
			else {
				//set background color
				std::cout << "\x1b[48;2;30;30;30m";

				//if cell is at the top world border
				if (map_y == world_size - 1) {
					std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
				}
				else {
					std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[16D\x1b[B";
				}

				//if cell has stench
				if (map.get_cell(map_x, map_y) & STENCH) {
					std::cout << "\xdb\xdb\x1b[33m\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\xb1\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
					std::cout << "\xdb\xdb\x1b[33m\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
				}
				else {
					std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
					std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
				}

				//if cell is at the left world border
				if (map_x == 0) {
					std::cout << "\xdb\xdb            \xd0\xd0\x1b[16D\x1b[B";
					std::cout << "\xdb\xdb            \xd2\xd2\x1b[16D\x1b[B";
				}
				else {
					std::cout << "\xd0\xd0      ";
					//if cell is at the right world border
					if (map_x == world_size - 1) {
						std::cout << "      \xdb\xdb\x1b[16D\x1b[B";
						std::cout << "\xd2\xd2            \xdb\xdb\x1b[16D\x1b[B";
					}
					else {
						std::cout << "      \xd0\xd0\x1b[16D\x1b[B";
						std::cout << "\xd2\xd2            \xd2\xd2\x1b[16D\x1b[B";
					}
				}

				//if the cell has the gold
				if (map.get_cell(map_x, map_y) & GOLD) {
					std::cout << "\x1b[6C\x1b[2A\x1b[33;1m\x1b[48;2;81;77;47m    \x1b[B\x1b[4D\xf0\xdc\xdb\xf0\x1b[10D\x1b[B\x1b[37;22m\x1b[48;2;30;30;30m";
				}

				//if the cell is breezy
				if (map.get_cell(map_x, map_y) & 3/*3 becuase it checks the first 2 bytes, and we can reach 3 breezes, but not 4*/) {
					std::cout << "\xdb\xdb\x1b[36m\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
					std::cout << "\xdb\xdb" << map_x << "," << map_y << "\x1b[36m";
					//adjustement for 2 space wide numbers
					for (short i = 0; i < (1 - (short)(map_x / 10)) + (map_x >= 0); i++) {
						std::cout << "\xb1";
					}
					for (short i = 0; i < (1 - (short)(map_y / 10)) + (map_y >= 0); i++) {
						std::cout << "\xb1";
					}
					std::cout << "\xb1\xb1\xb1\xb1\xb1\x1b[37m\xdb\xdb\x1b[16D\x1b[B";
				}
				else {
					std::cout << "\xdb\xdb            \xdb\xdb\x1b[16D\x1b[B";
					std::cout << "\xdb\xdb" << map_x << "," << map_y;
					//adjustement for 2 space wide numbers
					for (short i = 0; i < (1 - (short)(map_x / 10)) + (map_x >= 0); i++) {
						std::cout << " ";
					}
					for (short i = 0; i < (1 - (short)(map_y / 10)) + (map_y >= 0); i++) {
						std::cout << " ";
					}
					std::cout << "     \xdb\xdb\x1b[16D\x1b[B";
				}

				//if cell is at the lower world border
				if (map_y == 0) {
					std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
				}
				else {
					std::cout << "\xdb\xdb\xdb\xdb\xdb\xdb\xb5  \xc6\xdb\xdb\xdb\xdb\xdb\xdb\x1b[7A";
				}

				//draw agent over cell, is easier then always checking in every line (saves ifs)
				if (map.get_cell(map_x, map_y) & AGENT) {
					if (player_walk_animation) {
						player_walk_animation = 0;
						std::cout << "\x1b[32m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xda\xd7\xd9\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
					}
					else {
						player_walk_animation = 1;
						std::cout << "\x1b[32m\x1b[4B\x1b[4D\x01\x1b[1B\x1b[2D\xc0\xd7\xbf\x1b[1B\x1b[3D\xc9\xca\xbb\x1b[37m\x1b[6A\x1b[2C";
					}
				}

				if (map.get_cell(map_x, map_y) & GOLD) {
					//set end screen var plus disable movement
					got_gold = 1;
					if (playertype) {
						player.disable_walking();
					}
					else {
						mr_robot.disable_walking();
					}

					//left = 1, top = 2, right = 4, bottom = 8
					uint8_t border = 0, i_at_border_hit = 0;

					Sleep(500);

					//set colors plus move cursor to the correct position
					std::cout << "\x1b[33;1m\x1b[43;1m\x1b[" << ((3 - y) * 8) - 5 << ";" << x * 16 + 1 << "H\x1b[3B\x1b[6C";

					for (uint8_t i = 1; i < 6; i += 2) { //takes 4 + 1 cylces (<6) to fill the cell with the gold

						//some really fucked up shit that just miraculously works (sometimes) ¯\_(ツ)_/¯ v0.5
						//right --> left
						if (i == 1) {
							//fill central square
							std::cout << "\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D";
							std::cout << "\x1b[B\x1b[2C\xdb\xdb\x1b[4D\xdb\xdb\x1b[2D\x1b[2A";
						}
						else {
							std::cout << "\x1b[A\x1b[2D";
						}
						std::cout << "\xdb\xdb\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\xdb\xdb";
						}

						//top --> bottom
						std::cout << "\x1b[B\x1b[2D\xdb\xdb";
						std::cout << "\x1b[B\x1b[2D\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\x1b[B\x1b[2D\xdb\xdb";
						}

						//left --> right
						std::cout << "\x1b[4D\xdb\xdb";
						std::cout << "\x1b[4D\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\x1b[4D\xdb\xdb";
						}

						//bottom --> top
						std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						for (uint8_t k = 0; k < i; k++) {
							std::cout << "\x1b[A\x1b[2D\xdb\xdb";
						}

						Sleep(200 - i * 15);
					}

					double eight = 8;
					for (short i = 2; i >= 0; --i) {
						for (short j = 0; j < 3; j++) {
							std::cout << "\x1b[" << ((3 - i) * 8) - 5 << ";" << j * 16 + 1 << "H";
							draw_block(&eight);
						}
					}

					//vars for game end
					got_gold = true;
					gameover = true;
				}

				//reset background color + scrollbar
				ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
				std::cout << "\x1b[0m";
			}
		}
	}
}

//puts the map in the console
void wumpus::draw_map() {
	std::cout << "\x1b[2;0H";

	//iterate through all cells and output them
	for (short i = world_size - 1; i >= 0; --i) {
		for (short j = 0; j < world_size; j++) {
			draw_cell(j, i);
		}
	}

	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
}

//draw the 3x3 set of cells on a small screen
void wumpus::draw_map_small() {

	//edge cases in clockwise direction
	// MIN: < 2
	// MID: >= 2 && < world_size - 2
	// MAX: >= world_size - 2
	// 
	// XMIN YMIN ok
	// XMIN YMID ok
	// XMIN YMAX ok
	// XMID YMAX ok
	// XMAX YMAX ok
	// XMAX YMID ok
	// XMAX YMIN ok
	// XMID YMIN ok
	// default case:
	// XMID YMID ok

	// XMIN YMIN  | works!
	if (player.get_x_position() < 2 && player.get_y_position() < 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMIN YMIN\x1b""8";
		//iterate through a set of cells and output them
		for (short i = 2; i >= 0; --i) {
			for (short j = 0; j < 3; j++) {
				draw_cell_small(j, i, j, i);
			}
		}
	}
	// XMIN YMID  | works!
	else if (player.get_x_position() < 2 && player.get_y_position() >= 2 && player.get_y_position() < world_size - 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMIN YMID\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, j, player.get_y_position() + k);
			}
			k--;
		}
	}
	// XMIN YMAX  | works!
	else if (player.get_x_position() < 2 && player.get_y_position() >= world_size - 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMIN YMAX\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, j, world_size - k);
			}
			k++;
		}
	}
	// XMID YMAX  | works!
	else if (player.get_x_position() >= 2 && player.get_x_position() < world_size - 2 && player.get_y_position() >= world_size - 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMID YMAX\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			short l = -1;
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, player.get_x_position() + l, world_size - k);
				l++;
			}
			k++;
		}
	}
	// XMAX YMAX  | works!
	else if (player.get_x_position() >= world_size - 2 && player.get_y_position() >= world_size - 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMAX YMAX\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			short l = 3;
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, world_size - l, world_size - k);
				l--;
			}
			/*std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";49H\x1b[30m";
			double eight = 8;
			draw_block(&eight);
			std::cout << "\x1b[0m";*/
			k++;
		}
	}
	// XMAX YMID  | works!
	else if (player.get_x_position() >= world_size - 2 && player.get_y_position() >= 2 && player.get_y_position() < world_size - 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMAX YMID\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			short l = 3;
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, world_size - l, player.get_y_position() + k);
				l--;
			}
			/*std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";49H\x1b[30m";
			double eight = 8;
			draw_block(&eight);
			std::cout << "\x1b[0m";*/
			k--;
		}
	}
	// XMAX YMIN  | works!
	else if (player.get_x_position() >= world_size - 2 && player.get_y_position() < 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMAX YMIN\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			short l = 3;
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, world_size - l, i);
				l--;
			}
			/*std::cout << "\x1b[" << ((3 - y) * 8) - 5 << ";49H\x1b[30m";
			double eight = 8;
			draw_block(&eight);
			std::cout << "\x1b[0m";*/
			k--;
		}
	}
	// XMID YMIN  | works!
	else if (player.get_x_position() >= 2 && player.get_y_position() < 2) {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMID YMIN\x1b""8";
		//iterate through a set of cells and output them
		short k = 1;
		for (short i = 2; i >= 0; --i) { //y loop
			short l = -1;
			for (short j = 0; j < 3; j++) { //x loop
				draw_cell_small(j, i, player.get_x_position() + l, i);
				l++;
			}
			k--;
		}
	}
	// default case XMID YMID | works!
	else {
		//std::cout << "\x1b""7\x1b[1;112H MODE: XMID YMID\x1b""8";
		short k = 1;
		for (short i = 2; i >= 0; --i) {
			short l = -1;
			for (short j = 0; j < 3; j++) {
				draw_cell_small(j, i, player.get_x_position() + l, player.get_y_position() + k);
				l++;
			}
			k--;
		}
	}

	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
}

//redraws the last visited cells
void wumpus::redraw_map() {
	if (small_screen) {
		draw_map_small();
	}
	else {
		if (playertype) {
			draw_cell(player.get_x_position(), player.get_y_position());
		}
		else {
			draw_cell(mr_robot.get_x_position(), mr_robot.get_y_position());
		}
	}

	//disable cursor
	std::cout << "\x1b[?12l\x1b[?25l";
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);
}

//restart checker function
bool wumpus::check_for_restart() {
	while (!(GetKeyState(VK_ESCAPE) & 0x8000)) {
		if (GetKeyState(VK_RETURN) & 0x8000) {
			if (!esc_menu_request) {
				clear_screen();
				gameover = 0;
				fell_in_pit = 0;
				met_wumpus = 0;
				got_gold = 0;
				esc_menu_request = 0;
				map.clear();
			}
			return true;
		}
	}
	return false;
}

//main function
int main() {
	short x, y;
	wumpus game;

	//game init stuff, might be moved to a ingame menu
	game.playertype = 1; // 0 = ai, 1 = local player, aka human, 2 or more = ?
	game.world_size = 10;

	//we can create the image files if we need
	if (game.initialize_files()) {

		game.initialize_console();

		//goto label for restarting
	restart_entry:

		game.initialize_map();
		//game.clear_screen();

		//draw map depending on screen size
		if (game.small_screen) {
			game.draw_map_small();
		}
		else {
			game.draw_map();
		}

		//if we have a human player
		if (game.playertype) {
			//init player
			game.player.set_x_position(0);
			game.player.set_y_position(0);

			//return entry for the player agent
		menu_entry_player_agent:

			game.player.enable_walking();

			//just to update once
			game.redraw_map();

			//main game loop
			while (!game.gameover && !game.esc_menu_request) {
				Sleep(33);
				//as long as nothing bad happens
				if (!game.met_wumpus && !game.fell_in_pit && !game.got_gold) {
					//input buffering/blocking is implemented in the agent class.
					game.player.walk(&x, &y, &game.old_x, &game.old_y, &game.world_size);
					//if the player did walk
					if (x != game.old_x || y != game.old_y) {
						game.map.update(x, y, game.old_x, game.old_y);
						game.redraw_map();
					}
				}
				else {
					game.redraw_map();
				}

				//goto splash screen on esc...
				if (GetAsyncKeyState(VK_ESCAPE)) {
					game.esc_menu_request = true;
					game.player.disable_walking();
				}
			}
		}
		else {
			//init ai player
			game.mr_robot.set_world_size(game.world_size);
			game.mr_robot.set_algorithm(0); //random walking
			game.mr_robot.initialize_memory();
			game.mr_robot.set_x_position(0);
			game.mr_robot.set_y_position(0);
			game.mr_robot.disable_walking();

			game.draw_message("PRESS RETURN TO START THE COMPUTER AGENT");

			//wait for input
			while (!GetAsyncKeyState(VK_RETURN)) {} //only works first time, dunno

			//return entry for the ai agent
		menu_entry_ai_agent:

			game.mr_robot.enable_walking();

			//remove message
			std::cout << "\x1b[""7\x1b[2;0H\x1b[2K\x1b""8";

			//just to update once
			game.redraw_map();

			//main game loop
			while (!game.gameover && !game.esc_menu_request) {
				Sleep(33);
				//as long as nothing bad happens
				if (!game.met_wumpus && !game.fell_in_pit && !game.got_gold) {
					//input buffering/blocking is implemented in the agent class.
					game.mr_robot.step(&x, &y, &game.old_x, &game.old_y);
					//if the player did walk
					if (x != game.old_x || y != game.old_y) {
						game.map.update(x, y, game.old_x, game.old_y);
						game.redraw_map();
					}
				}
				else {
					game.redraw_map();
				}

				//goto splash screen on esc...
				if (GetAsyncKeyState(VK_ESCAPE)) {
					game.esc_menu_request = true;
					game.mr_robot.disable_walking();
				}
			}
		}

		//draw game end screen
		game.draw_gameover();

		//check for restart and resume at the correct location
		if (game.check_for_restart()) {
			if (game.esc_menu_request) {
				game.esc_menu_request = false;
				if (game.playertype) {
					goto menu_entry_player_agent;
				}
				else {
					goto menu_entry_ai_agent;
				}
			}
			else {
				goto restart_entry;
			}
		}
	}

	//exit the program
	ExitProcess(0);
}