#ifndef minesweeper_h
#define minesweeper_h

#include <iostream>

int main();
int action_show(char * board, int width, int height, bool & mines_placed, int mine_number);
void unhide(char * board, int width, int height, int xloc, int yloc);
void action_mark(char * board, int width, int height);
int cin_int();
void start_game(char * & board, int & width, int & height, int & mine_number);
char * create_board(int width, int height);
void place_mines(char * board, int width, int height, int safe_location, int mine_number);
void print_board(char * board, int width, int height);
char cin_char();
void unhide_all(char * board, int width, int height);
bool is_game_won(char * board, int width, int height);
void compute_adjacent(char * board, int width, int height);
void check_tile(char * board, int index, int width, int height, int xpos, int ypos);

#endif