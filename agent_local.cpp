#include "agent_local.h"

void agent_local::walk(short* new_x, short* new_y, short* old_x, short* old_y) {

	*old_x = agent_base::get_x_position();
	*old_y = agent_base::get_y_position();
	if (GetKeyState('W') & 0x8000) {
		agent_base::set_x_position(agent_base::get_x_position() + 1);
	}
	else if (GetKeyState('A') & 0x8000) {
		agent_base::set_y_position(agent_base::get_y_position() - 1);
	}
	else if (GetKeyState('S') & 0x8000) {
		agent_base::set_x_position(agent_base::get_x_position() - 1);
	}
	else if (GetKeyState('D') & 0x8000) {
		agent_base::set_y_position(agent_base::get_y_position() + 1);
	}
	*new_x = agent_base::get_x_position();
	*new_y = agent_base::get_y_position();
}