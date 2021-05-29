#pragma once
#ifndef WUMPUS_H
#define WUMPUS_H

#define	AGENT	128
#define	BREEZE	1
#define	GOLD	8
#define	PIT		16
#define	STENCH	32
#define	WUMPUS	64
#define	VISITED 512


//own libraries
#include "world.h"
#include "agent_auto.h"
#include "agent_local.h"
#include "images.h"

//std. libraries
#include <iostream>
#include <random>
#include <time.h>
#include <windows.h> //well on windows obviously it's standard, fight me

//ext. libraries
#include "bmp/BMP.h"

class wumpus {
public:
    wumpus() {};
    ~wumpus() {};
    BOOL WINAPI ConsoleHandler(DWORD ctrl_type);
    bool initialize_files();
    void initialize_map();
    void initialize_console();
    void clear_screen();
    void pit_ending_draw(short x, short y);
    void draw_block(double* size);
    void draw_from_bmp(char* path);
    void draw_gameover();
    void draw_cell(short x, short y);
    void draw_cell_small(short x, short y, short map_x, short map_y);
    void draw_map();
    void draw_map_small();
    void redraw_map();
    bool check_for_restart();
    short world_size = 10, old_x = 0, old_y = 0, playertype = 0; //different useful? variables
    world map;
    agent_local player;
    agent_auto mr_robot;
    bool player_walk_animation = 0, gameover = 0, fell_in_pit = 0, met_wumpus = 0, got_gold = 0, esc_ended = 0, small_screen = 0;
private:
};

#endif // !WUMPUS_H