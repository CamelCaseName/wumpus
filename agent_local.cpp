#include "agent_local.h"

void agent_local::walk(short* new_x, short* new_y, short* old_x, short* old_y, short* world_size) {
	//doing some funky direction adjustements here because i flipped the playing field somewhere...
	*old_x = agent_base::get_x_position();
	*old_y = agent_base::get_y_position();
	//time.h defines CLOCKS_PER_SEC as 1000, so i just don't use it for ms precission
	if (agent_base::get_walking_enabled_state() && ((clock() - last_input) > 400)) {
		if (((GetKeyState('D') & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000)) && agent_base::get_x_position() < (*world_size - 1)) {
			last_input = clock();
			agent_base::set_x_position(agent_base::get_x_position() + 1);
		}
		else if (((GetKeyState('S') & 0x8000) || (GetKeyState(VK_DOWN) & 0x8000)) && agent_base::get_y_position() != 0) {
			last_input = clock();
			agent_base::set_y_position(agent_base::get_y_position() - 1);
		}
		else if (((GetKeyState('A') & 0x8000) || (GetKeyState(VK_LEFT) & 0x8000)) && agent_base::get_x_position() != 0) {
			last_input = clock();
			agent_base::set_x_position(agent_base::get_x_position() - 1);
		}
		else if (((GetKeyState('W') & 0x8000) || (GetKeyState(VK_UP) & 0x8000)) && agent_base::get_y_position() < (*world_size - 1)) {
			last_input = clock();
			agent_base::set_y_position(agent_base::get_y_position() + 1);
		}
	}
	*new_x = agent_base::get_x_position();
	*new_y = agent_base::get_y_position();
}