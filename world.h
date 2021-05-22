#pragma once
#ifndef WORLD_H
#define WORLD_H


#include "cell.h"
#include <vector>
#include <iostream>

class world {
public:
	//fill world with empty cell on creation
	world() {};
	//world destructor
	~world() {};
	void set_size(uint16_t size);
	void set_cell(uint16_t y, uint16_t x, uint16_t attributes);
	uint16_t get_cell(uint16_t y, uint16_t x);

private:
	std::vector <std::vector <cell> > internal_map;
};

#endif // !WORLD_H