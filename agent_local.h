#pragma once
#ifndef AGENT_LOCAL_H
#define AGENT_LOCAL_H

#include "agent_base.h"
#include <time.h>

class agent_local : public agent_base {
public:
    agent_local() {};
    ~agent_local() {};
    void walk(short* new_x, short* new_y, short* old_x, short* old_y, short* world_size);
private:
    clock_t last_input = clock();
};
#endif // !AGENT_LOCAL_H