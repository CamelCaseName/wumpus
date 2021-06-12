#include "wumpus.h"

//global vartiables bad, but hey, no namespace pollution so far
short x, y;
wumpus game;

void menu_entry_player_agent() {
	game.player.enable_walking();

	//just to update once
	game.redraw_map();

	//main game loop
	while (!game.gameover) {
		Sleep(33);
		//as long as nothing bad happens
		if (!game.met_wumpus && !game.fell_in_pit && !game.got_gold) {
			//input buffering/blocking is implemented in the agent class.
			game.player.walk(&x, &y, &game.old_x, &game.old_y, &game.world_size);
			//if the player did walk
			if (x != game.old_x || y != game.old_y) {
				game.map.update(x, y, game.old_x, game.old_y);
				game.redraw_map();
			}
		}
		else {
			game.redraw_map();
		}

		//goto splash screen on esc...
		if (GetAsyncKeyState(VK_ESCAPE)) {
			game.esc_menu_request = true;
			game.player.disable_walking();
			game.draw_gameover();
			if (!game.gameover) {
				game.esc_menu_request = false;
				menu_entry_player_agent();
			}
		}
	}
}

void menu_entry_ai_agent() {

	game.mr_robot.enable_walking();

	//remove message
	std::cout << "\x1b[""7\x1b[2;0H\x1b[2K\x1b""8";

	//just to update once
	game.redraw_map();

	//main game loop
	while (!game.gameover) {
		Sleep(33);
		//as long as nothing bad happens
		if (!game.met_wumpus && !game.fell_in_pit && !game.got_gold) {
			//input buffering/blocking is implemented in the agent class.
			game.mr_robot.step(&x, &y, &game.old_x, &game.old_y);
			//if the player did walk
			if (x != game.old_x || y != game.old_y) {
				game.map.update(x, y, game.old_x, game.old_y);
				game.redraw_map();
			}
		}
		else {
			game.redraw_map();
		}

		//goto splash screen on esc...
		if (GetAsyncKeyState(VK_ESCAPE)) {
			game.esc_menu_request = true;
			game.mr_robot.disable_walking();
			game.draw_gameover();
			if (!game.gameover) {
				game.esc_menu_request = false;
				menu_entry_ai_agent();
			}
		}
	}
}

void restart_entry() {

	game.clear_screen();
	game.initialize_map();
	game.initialize_console(); //just to adjust the framebuffer and text and everything

	//draw map depending on screen size
	if (game.small_screen) {
		game.draw_map_small();
	}
	else {
		game.draw_map();
	}

	//if we have a human player
	if (game.playertype) {
		//init player
		game.player.set_x_position(0);
		game.player.set_y_position(0);

		//return entry for the player agent
		menu_entry_player_agent();
	}
	else {
		//init ai player
		game.mr_robot.set_world_size(game.world_size);
		game.mr_robot.initialize_memory();
		game.mr_robot.set_x_position(0);
		game.mr_robot.set_y_position(0);
		game.mr_robot.disable_walking();

		game.draw_message("PRESS RETURN TO START THE COMPUTER AGENT");

		//wait for input
		while (!GetAsyncKeyState(VK_RETURN)) {} //only works first time, dunno

		//return entry for the ai agent
		menu_entry_ai_agent();
	}

	if (game.gameover && game.esc_menu_request) {
		game.gameover = game.esc_menu_request = false;
		restart_entry();
	}
	else {
		//draw game end screen
		game.draw_gameover();
	}

	//check for restart and resume at the correct location
	if (game.check_for_restart()) {
		restart_entry();
	}

}

//main function
int main() {

	//game init stuff, moved to a ingame menu (in progress?...)
	game.playertype = 1; // 0 = ai, 1 = local player, aka human, 2 or more = ?
	game.world_size = 4;
	game.mr_robot.set_algorithm(0); //random walking = 0, better? = 1, no more yet

	if (game.initialize_files()) {

		game.initialize_console();

		//open settings to adjust them 
		game.esc_menu_request = true;
		game.draw_gameover();
		game.esc_menu_request = false;

		restart_entry();//contains the "main game loop"
	}

	//exit the program
	ExitProcess(0);
}