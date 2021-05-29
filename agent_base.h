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
	void enable_walking() { walking_enabled = true; };
	void disable_walking() { walking_enabled = false; };
	bool get_walking_enabled_state() {return walking_enabled;};
private:
	short x_position, y_position;	 
	bool walking_enabled = false;
};

#endif // !AGENT_BASE_H