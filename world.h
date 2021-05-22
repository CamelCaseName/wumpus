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
	void set_size(short size);
	void set_cell(short y, short x, short attributes);
	short get_cell(short y, short x);

private:
	std::vector <std::vector <cell> > internal_map;
};

#endif // !WORLD_H