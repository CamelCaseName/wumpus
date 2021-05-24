#include "agent_local.h"

void agent_local::walk(short* new_x, short* new_y, short* old_x, short* old_y, short* world_size) {
	//doing some funky direction adjustements here becuase i flipped the playing field somewhere...
	*old_x = agent_base::get_x_position();
	*old_y = agent_base::get_y_position();
	if (walking_enabled) {
		if ((GetKeyState('D') & 0x8000) && agent_base::get_x_position() < (*world_size - 1)) {
			agent_base::set_x_position(agent_base::get_x_position() + 1);
		}
		else if ((GetKeyState('S') & 0x8000) && agent_base::get_y_position() != 0) {
			agent_base::set_y_position(agent_base::get_y_position() - 1);
		}
		else if ((GetKeyState('A') & 0x8000) && agent_base::get_x_position() != 0) {
			agent_base::set_x_position(agent_base::get_x_position() - 1);
		}
		else if ((GetKeyState('W') & 0x8000) && agent_base::get_y_position() < (*world_size - 1)) {
			agent_base::set_y_position(agent_base::get_y_position() + 1);
		}
	}
	*new_x = agent_base::get_x_position();
	*new_y = agent_base::get_y_position();
}