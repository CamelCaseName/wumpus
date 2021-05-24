#pragma once
#ifndef WORLD_H
#define WORLD_H

#define	AGENT	128
#define	BREEZE	1
#define	GOLD	8
#define	PIT		16
#define	STENCH	32
#define	WUMPUS	64
#define	VISITED 512

#include "cell.h"
#include <vector>
#include <iostream>

class world {
public:
	short size;
	//fill world with empty cell on creation;
	world() { size = 0; };
	world(short size_in) { set_size(size_in); };
	//world destructor
	~world() {};
	void set_size(short size_in);
	void set_cell(short y, short x, short attributes);
	short get_cell(short y, short x);
	void update(short x = 0, short y = 0, short old_x = 0, short old_y = 0);
	bool size_check(short x, short y);
	bool size_check(short x, short y, short x2, short y2);
	bool random_fill();
	bool default_fill();
	void clear();

private:
	std::vector <std::vector <cell> > internal_map;
};

#endif // !WORLD_H