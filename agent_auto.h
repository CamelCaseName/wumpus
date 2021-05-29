#pragma once
#ifndef AGENT_AUTO_H
#define AGENT_AUTO_H

#include "agent_base.h"
#include "node.h"
#include <vector>

class agent_auto : public agent_base {
public:
    agent_auto() {};
    ~agent_auto() {};
    void get_cell_attributes(short x, short y, short attributes);
    void step(short* new_x, short* new_y, short* old_x, short* old_y);
    void set_world_size(short world_size_in){world_size = world_size_in;};
private:
    void move_up();
    void move_right();
    void move_down();
    void move_left();
    short world_size = 0;
    //vector<node> closed_list;
    //vector<node> open_list;
};

#endif // !AGENT_AUTO_H