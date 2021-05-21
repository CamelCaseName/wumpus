#pragma once
#ifndef CELL_H
#define CELL_H

#include <stdint.h>

class cell {
public:
	cell(uint8_t beginning_attributes) { attributes = beginning_attributes; };
	~cell(){};
	uint8_t get_attributes() { return attributes; };
	void set_attributes(uint8_t new_attributes) { attributes = new_attributes; };
private:
	uint8_t attributes = 0;
};


#endif // !CELL_H