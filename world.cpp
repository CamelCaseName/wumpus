#include "world.h"
//#define DEBUG

std::vector <std::vector <cell> > internal_map;

//checks coords on world size accuracy
bool world::size_check(short x, short y) {
	if (x < (short)internal_map.size() && x >= 0) {
		if (y < (short)internal_map[x].size() && y >= 0) {
			return true;
		}
		else return false;
	}
	else return false;
}

//checks coords on world size accuracy with 2 coords
bool world::size_check(short x, short y, short x2, short y2) {
	if (x < (short)internal_map.size() && x >= 0) {
		if (y < (short)internal_map[x].size() && y >= 0) {
			if (x2 < (short)internal_map.size() && x2 >= 0) {
				if (y2 < (short)internal_map[x].size() && y2 >= 0) {
					return true;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}

//sets the attributes of the cell at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
void world::set_cell(short x, short y, short attributes) {
	if (size_check(x, y)) {
#ifdef DEBUG
		std::cout << "setting cell @ " << (int)y << "," << (int)x << " to: " << (int)attributes;
#endif // !DEBUG
		internal_map[x][y].set_attributes(attributes);
#ifdef DEBUG
		if (internal_map[x][y].get_attributes() == attributes) {
			std::cout << "  | ok!" << std::endl;
		}
		else std::cout << "  | error!" << std::endl;
#endif // !DEBUG
	}
}

//returns the attributes of the cell as a short at the given x and y coordinates, 
//if they are outside the world boundary use the outermost cell
short world::get_cell(short x, short y) {
	//15th bit as error bit
	short retval = 0x4000;

	if (size_check(x, y)) {
		retval = internal_map[x][y].get_attributes();
	}

	return retval;
}

//updates the map when the player has moved, uncovers cells and so on
void world::update(short x, short y, short old_x, short old_y) {
	if (size_check(x, y, old_x, old_y)) {
		internal_map[x][y].set_attributes(AGENT);
		if (!(internal_map[x][y].get_attributes() & VISITED)) {
			internal_map[x][y].set_attributes(VISITED);
		}
		internal_map[old_x][old_y].set_attributes(AGENT);
	}
}

//fills the map with random cells, according to the rules (mostly)
bool world::random_fill() {
	// initialize random seed
	srand((uint32_t)(time(NULL)));

	//variables
	bool wumpus_set = false;
	bool gold_set = false;
	bool max_pits_set = false;
	short number_of_pits = 0;

	//create player on the map at 0,0
	set_cell(0, 0, AGENT + VISITED);

	//computer magic thingy
	for (short i = 0; i < size; i++) {
		for (short j = 0; j < size; j++) {
			//get current attributes to not overwrite anything
			//not in the starting corner
			if (i + j > 1) {
				//if no wumpus exists yet
				if (!wumpus_set && !(get_cell(i, j) & GOLD) && !(get_cell(i, j) & PIT)) {
					//			  random between 1 and all remaining squares equal to 1
					wumpus_set = ((rand() % ((size * size) - (((int)i + 1) * ((int)j + 1)) + 1)) <= size + 1);
					//set cell to wumpus
					if (wumpus_set) {
						set_cell(i, j, WUMPUS);
						//set cells around the wumpus
						set_cell(i + 1, j, STENCH);
						set_cell(i, j + 1, STENCH);
						set_cell(i - 1, j, STENCH);
						set_cell(i, j - 1, STENCH);
					}
				}
				//if we have no gold cell already and no wumpus on the same cell, create gold
				if (!gold_set && !(get_cell(i, j) & WUMPUS) && !(get_cell(i, j) & PIT)) {
					gold_set = ((rand() % ((size * size) - ((i + 1) * (j + 1)) + 1)) <= size + 2);
					if (gold_set) {
						//set cell to gold
						set_cell(i, j, GOLD);
					}
				}
				//if we have no wumpus and gold, dig the pit on this cell
				if (!max_pits_set && !(get_cell(i, j) & WUMPUS) && !(get_cell(i, j) & GOLD) && !(get_cell(i, j) & PIT)) {
					if (((rand() % ((size * size) - ((i + 1) * (j + 1)) + 1)) <= size + 1)) {
						//if current amount of pits is greater than 1/5th of cells, we are done with them
						if (++number_of_pits >= (size - 1)) {
							max_pits_set = true;
#ifdef DEBUG
							std::cout << "max number of pits reached" << std::endl;
#endif // !DEBUG
						}
						//set cell to pit
						set_cell(i, j, PIT);
						//set cells around the pit to breeze
						set_cell(i + 1, j, BREEZE);
						set_cell(i, j + 1, BREEZE);
						set_cell(i - 1, j, BREEZE);
						set_cell(i, j - 1, BREEZE);
					}
				}
			}
		}
	}

	//check for map validity
	if (wumpus_set && gold_set) {
		return true;
	}
	//if not correct, fill the map with the default setting in any size
	else {
		default_fill();
		return true;
	}
	//need some error case for the future, will see
}

//fills the map with the default configuration given in the pdf
bool world::default_fill() {
	set_cell(0, 0, AGENT + VISITED);
	set_cell(0, 2, WUMPUS);
	set_cell(1, 2, GOLD);
	set_cell(2, 2, PIT);
	set_cell(2, 0, PIT);
	set_cell(3, 3, PIT);
	set_cell(1, 0, BREEZE);
	set_cell(3, 0, BREEZE);
	set_cell(2, 1, BREEZE);
	set_cell(1, 2, BREEZE);
	set_cell(2, 3, BREEZE);
	set_cell(1, 2, BREEZE);
	set_cell(1, 2, STENCH);
	set_cell(0, 1, STENCH);
	set_cell(0, 3, STENCH);
	return true;
}

//creates a 2d vector off cells according to the size (i.e. the square side length)
void world::set_size(short size_in) {
	size = size_in;
	//temporary objects to fill the map with
	cell default_cell(0);
	std::vector <cell> default_vector_of_cells;

	//push back size amount of cells per column
	for (short i = 0; i < size; i++) {
		default_vector_of_cells.push_back(default_cell);
	}

	//push back size amount of columns into the map
	for (short i = 0; i < size; i++) {
		internal_map.push_back(default_vector_of_cells);
	}
}

void world::clear() {

	//pop all cells
	for (short i = 0; i < size; i++) {
		internal_map[i].clear();
	}
	internal_map.clear();
}