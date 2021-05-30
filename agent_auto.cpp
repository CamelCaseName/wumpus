# include "agent_auto.h"

void agent_auto::get_cell_attributes(short attributes) {
    map_memory[agent_base::get_x_position()][agent_base::get_y_position()].set_attributes(attributes - 640);
}

void agent_auto::initialize_memory() {
    //temporary objects to fill the map with
    cell default_cell(0);
    std::vector <cell> default_vector_of_cells;

    //push back size amount of cells per column
    for (short i = 0; i < world_size; i++) {
        default_vector_of_cells.push_back(default_cell);
    }

    //push back size amount of columns into the map
    for (short i = 0; i < world_size; i++) {
        map_memory.push_back(default_vector_of_cells);
    }
}

void agent_auto::clear_memory() {
    for (short i = 0; i < world_size; i++) {
        map_memory[i].clear();
    }
    map_memory.clear();
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
    if (!algorithm && get_walking_enabled_state()) {
        switch (rand() % 5) {
        case 0:
            move_up();
            Sleep(300);
            break;
        case 1:
            move_left();
            Sleep(300);
            break;
        case 2:
            move_down();
            Sleep(300);
            break;
        case 3:
            move_right();
            Sleep(300);
            break;
        case 4:
            Sleep(300);
            break;
        default:
            Sleep(300);
            break;
        }
    }
    else if (algorithm == 1 && get_walking_enabled_state()) {
        if (map_memory[agent_base::get_x_position()][agent_base::get_y_position()].get_attributes()) {

        }
    }


    *new_x = agent_base::get_x_position();
    *new_y = agent_base::get_y_position();
}