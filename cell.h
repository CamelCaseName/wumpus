#pragma once
#ifndef CELL_H
#define CELL_H

#include <stdint.h>

class cell {
public:
	cell(short beginning_attributes) { attributes = beginning_attributes; };
	~cell(){};
	short get_attributes() { return attributes; };
	void set_attributes(short new_attributes) { attributes = new_attributes; };
private:
	short attributes = 0;
};


#endif // !CELL_H