#pragma once
#ifndef AGENT_AUTO_H
#define AGENT_AUTO_H

#include "agent_base.h"
#include "cell.h"
#include <vector>
#include <random>

class agent_auto : public agent_base {
public:
    agent_auto() {};
    ~agent_auto() {};
    void get_cell_attributes(short attributes);
    void step(short* new_x, short* new_y, short* old_x, short* old_y);
    void set_world_size(short world_size_in) { world_size = world_size_in; };
    void set_algorithm(short algo) { algorithm = algo; };
    void initialize_memory();
    void clear_memory();
private:
    void move_up();
    void move_right();
    void move_down();
    void move_left();
    short world_size = 0, algorithm = 0;
    std::vector<std::vector<cell>> map_memory;
};

#endif // !AGENT_AUTO_H