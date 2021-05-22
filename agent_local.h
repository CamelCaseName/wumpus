#pragma once
#ifndef AGENT_LOCAL_H
#define AGENT_LOCAL_H

#include "agent_base.h"

class agent_local : public agent_base{
public:
	agent_local() {};
	~agent_local() {};
	void walk(short* new_x, short* new_y, short* old_x, short* old_y);
private:

};
#endif // !AGENT_LOCAL_H