#pragma once
#ifndef AGENT_BASE_H
#define AGENT_BASE_H

#include <Windows.h>

class agent_base {
public:
	agent_base() { x_position = 0; y_position = 0; };
	~agent_base() {};
	void set_x_position(short x_in) { x_position = x_in; };
	void set_y_position(short y_in) { y_position = y_in; };
	short get_x_position() { return x_position; };
	short get_y_position() { return y_position; };
private:
	short x_position, y_position;
};

#endif // !AGENT_BASE_H