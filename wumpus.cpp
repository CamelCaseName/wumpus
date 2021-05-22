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

short world_size = 4;
world map;
agent_local player;


void initialize_map() {
	map.set_size(world_size);
}

BOOL WINAPI ConsoleHandler(DWORD ctrl_type) {
	if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
		ExitProcess(0);
	}
	return FALSE;
}

void fill_map() {
	// initialize random seed
	srand((uint32_t)(time(NULL)));

	//variables
	bool wumpus_set = false;
	bool gold_set = false;
	bool max_pits_set = false;
	short number_of_pits = 0;

	//create player on the map at 0,0
	map.set_cell(0, 0, 128);

	//computer magic thingy
	for (short i = 0; i < world_size; i++) {
		for (short j = 0; j < world_size; j++) {
			//get current attributes to not overwrite anything
			//not in the starting corner
			if (i + j > 1) {
				//if no wumpus exists yet
				if (!wumpus_set && !(map.get_cell(i, j) & 8) && !(map.get_cell(i, j) & 16)) {
					//			  random between 1 and all remaining squares equal to 1
					wumpus_set = ((rand() % ((world_size * world_size) - (((int)i + 1) * ((int)j + 1)) + 1)) <= world_size + 1);
					//set cell to wumpus
					if (wumpus_set) {
						map.set_cell(i, j, map.get_cell(i, j) + 64);
						//set cells around the wumpus
						map.set_cell(i + 1, j, map.get_cell(i + 1, j) + 32);
						map.set_cell(i, j + 1, map.get_cell(i, j + 1) + 32);
						map.set_cell(i - 1, j, map.get_cell(i - 1, j) + 32);
						map.set_cell(i, j - 1, map.get_cell(i, j - 1) + 32);
					}
				}
				//if we have no gold cell already and no wumpus on the same cell, create gold
				if (!gold_set && !(map.get_cell(i, j) & 64) && !(map.get_cell(i, j) & 16)) {
					gold_set = ((rand() % ((world_size * world_size) - ((i + 1) * (j + 1)) + 1)) <= world_size + 1);
					if (gold_set) {
						//set cell to gold
						map.set_cell(i, j, map.get_cell(i, j) + 8);
					}
				}
				//if we have no wumpus and gold, dig the pit on this cell
				if (!max_pits_set && !(map.get_cell(i, j) & 64) && !(map.get_cell(i, j) & 8)) {
					if (((rand() % ((world_size * world_size) - ((i + 1) * (j + 1)) + 1)) <= world_size + 1)) {
						//if current amount of pits is greater than 1/5th of cells, we are done with them
						if (++number_of_pits >= (world_size - 1)) {
							max_pits_set = true;
#ifdef DEBUG
							std::cout << "max number of pits reached" << std::endl;
#endif // !DEBUG
						}
						//set cell to pit
						map.set_cell(i, j, map.get_cell(i, j) + 16);
						//set cells around the pit to breeze
						map.set_cell(i + 1, j, map.get_cell(i + 1, j) + 1);
						map.set_cell(i, j + 1, map.get_cell(i, j + 1) + 1);
						map.set_cell(i - 1, j, map.get_cell(i - 1, j) + 1);
						map.set_cell(i, j - 1, map.get_cell(i, j - 1) + 1);
					}
				}
			}
		}
	}
}

void draw_map() {
	for (short i = world_size; i > 0; --i) {
		for (short j = 0; j < world_size; j++) {
			std::cout << (int)j << "," << (int)i - 1 << ":" << (int)map.get_cell(i - 1, j) << " ";
		}
		std::cout << std::endl;
	}
}

void clear_screen(char fill = ' ') {//kindly borrowed from https://stackoverflow.com/questions/5866529/how-do-we-clear-the-console-in-assembly/5866648#5866648
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

int main() {
	short x, y, old_x, old_y;
	initialize_map();

	fill_map();

	while (1) {
		Sleep(1000);
		//clear_screen();
		draw_map();
		player.walk(&x, &y, &old_x, &old_y);
		if (x != old_x || y != old_y) {
			map.set_cell(x, y, AGENT);
			map.set_cell(old_x, old_y, (map.get_cell(old_x, old_y) - AGENT));
		}

	}
}
