# include "agent_auto.h"

void agent_auto::get_cell_attributes(short x, short y, short attributes){

}

void agent_auto::step(short* new_x, short* new_y, short* old_x, short* old_y) {
    *old_x = agent_base::get_x_position();
    *old_y = agent_base::get_y_position();

    //  1. step (calc stuff for movement)
    //
    //  2. take in the scenery (get the cell attributes)
    //
    //  3. step again with new calculations
    //
    //  some algo stuff

    *new_x = agent_base::get_x_position();
    *new_y = agent_base::get_y_position();
}

void agent_auto::move_up() {
    if (agent_base::get_y_position() < (world_size - 1)) {
        agent_base::set_y_position(agent_base::get_y_position() + 1);
    }
}

void agent_auto::move_right() {
    if (agent_base::get_x_position() < (world_size - 1)) {
        agent_base::set_x_position(agent_base::get_x_position() + 1);
    }
}

void agent_auto::move_down() {
    if (agent_base::get_y_position() != 0) {
        agent_base::set_y_position(agent_base::get_y_position() - 1);
    }
}

void agent_auto::move_left() {
    if (agent_base::get_x_position() != 0) {
        agent_base::set_x_position(agent_base::get_x_position() - 1);
    }
}



