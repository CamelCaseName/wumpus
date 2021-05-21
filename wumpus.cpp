#include "wumpus.h"

//console size 80*24 chars
//create frame buffer to output directly to screen
// data structure -> 2d vector array (scalability after compile, so i can change world size dynamically)?, use bit encoding for cell state.. 
// or use a list of cell objects which can have multiple neighboors,so kinda like a graph but also bit encoding because efficient.

//cell state encoding:
//the attributes are just the states added up, so one bit is one state
//all possible states:
//	Agent	1
//	Breeze	2
//	Gold	4
//	Pit		8
//	Stench	16
//	Wumpus	32

uint8_t world_size = 4;
world map;

void initialize_map() {
	map.set_size(world_size);
	uint8_t test_attributes = 16;
	for (uint8_t i = 0; i < world_size; i++) {
		for (uint8_t j = 0; j < world_size; j++) {
			map.set_cell(i, j, test_attributes);
		}
	}
}

int main() {
	initialize_map();
}
