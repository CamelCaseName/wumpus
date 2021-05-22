#include "world.h"
//#define DEBUG

std::vector <std::vector <cell> > internal_map;

//sets the attributes of the cell at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
void world::set_cell(uint16_t x, uint16_t y, uint16_t attributes) {
	if (x < internal_map.size()) {
		if (y < internal_map[x].size()) {

#ifdef DEBUG
			std::cout << "setting cell @ " << (int)y << "," << (int)x << " to: " << (int)attributes ;
#endif // !DEBUG

			internal_map[x][y].set_attributes(attributes);
#ifdef DEBUG
			if (internal_map[x][y].get_attributes() == attributes) {
				std::cout << "  | ok!" << std::endl;
			}else std::cout << "  | error!" << std::endl;
#endif // !DEBUG
		}
	}
}

//returns the attributes of the cell as a uint16_t at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
uint16_t world::get_cell(uint16_t x, uint16_t y) {
	//8th bit as error bit
	uint16_t retval = 128;

	if (x < internal_map.size()) {
		if (y < internal_map[x].size()) {
			retval = internal_map[x][y].get_attributes();
		}
	}

	return retval;
}

//creates a 2d vector off cells according to the size (i.e. the square side length)
void world::set_size(uint16_t size) {
	//temporary objects to fill the map with
	cell default_cell(0);
	std::vector <cell> default_vector_of_cells;

	//push back size amount of cells per column
	for (uint16_t i = 0; i < size; i++) {
		default_vector_of_cells.push_back(default_cell);
	}

	//push back size amount of columns into the map
	for (uint16_t i = 0; i < size; i++) {
		internal_map.push_back(default_vector_of_cells);
	}
}