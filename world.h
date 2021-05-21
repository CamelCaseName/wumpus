#pragma once
#ifndef WORLD_H
#define WORLD_H


#include "cell.h"
#include <vector>

class world {
public:
	//fill world with empty cell on creation
	world() {};
	//world destructor
	~world() {};
	void set_size(uint8_t size);
	void set_cell(uint8_t y, uint8_t x, uint8_t attributes);
	uint8_t get_cell(uint8_t y, uint8_t x);

private:
	std::vector <std::vector <cell> > internal_map;
};

#endif // !WORLD_H