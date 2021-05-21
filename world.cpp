#include "world.h"

std::vector <std::vector <cell> > internal_map;

//sets the attributes of the cell at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
void world::set_cell(uint8_t y, uint8_t x, uint8_t attributes) {
	if (y <= internal_map.size()) {
		if (x <= internal_map[y].size()) {
			internal_map[y][x].set_attributes(attributes);
		}
		else {
			internal_map[y][internal_map[y].size() - 1].set_attributes(attributes);
		}
	}
	else {
		if (x <= internal_map[internal_map.size() - 1].size()) {
			internal_map[internal_map.size() - 1][x].set_attributes(attributes);
		}
		else {
			internal_map[internal_map.size() - 1][internal_map[y].size() - 1].set_attributes(attributes);
		}
	}
}

//returns the attributes of the cell as a uint8_t at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
uint8_t world::get_cell(uint8_t y, uint8_t x) {
	uint8_t retval = 0;

	if (y <= internal_map.size()) {
		if (x <= internal_map[y].size()) {
			retval = internal_map[y][x].get_attributes();
		}
		else {
			retval = internal_map[y][internal_map[y].size() - 1].get_attributes();
		}
	}
	else {
		if (x <= internal_map[internal_map.size() - 1].size()) {
			retval = internal_map[internal_map.size() - 1][x].get_attributes();
		}
		else {
			retval = internal_map[internal_map.size() - 1][internal_map[y].size() - 1].get_attributes();
		}
	}

	return retval;
}

//creates a 2d vector off cells according to the size (i.e. the square side length)
void world::set_size(uint8_t size) {
	//temporary objects to fill the map with
	cell default_cell(0);
	std::vector <cell> default_vector_of_cells;

	//push back size amount of cells per column
	for (uint8_t i = 0; i < size; i++) {
		default_vector_of_cells.push_back(default_cell);
	}

	//push back size amount of columns into the map
	for (uint8_t i = 0; i < size; i++) {
		internal_map.push_back(default_vector_of_cells);
	}
}