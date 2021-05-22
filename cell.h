#pragma once
#ifndef CELL_H
#define CELL_H

#include <stdint.h>

class cell {
public:
	cell(uint16_t beginning_attributes) { attributes = beginning_attributes; };
	~cell(){};
	uint16_t get_attributes() { return attributes; };
	void set_attributes(uint16_t new_attributes) { attributes = new_attributes; };
private:
	uint16_t attributes = 0;
};


#endif // !CELL_H