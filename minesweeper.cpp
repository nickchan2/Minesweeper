/*
 * minesweeper.cpp
 *  
 * Written by Nicholas Chan
 *  
 * g++ -std=c++14 -o ./minesweeper ./minesweeper.cpp && ./minesweeper
 *  
 * A text based minesweeper game
 * 
 * The board is 1d array of chars where bits
 *   7 indicates a mine tile
 *   6 indicates the tile is hidden
 *   5 indicates the tile is marked
 *   3-0 is the amount of adjacent tiles that contain mines
 *   (4 is unused)
 * 
 */

#include <iostream>
#include "minesweeper.hpp"

#define MAX_WIDTH 60
#define MAX_HEIGHT 40

#define HIDDEN_BIT 0X40
#define MARKED_BIT 0X20
#define MINE_BIT 0X80

int main() {

    int game_status{};

    do {

        std::system("clear");
        game_status = 0;
        int width{}, height{}, mine_number{};
        char * board{};
        bool mines_placed{false};

        start_game(board, width, height, mine_number);

        while(!game_status) {

            std::system("clear");
            print_board(board, width, height);
            std::cout << std::endl << "[S]how, [M]ark, [R]estart, [Q]uit" << std::endl;
            char action{cin_char()};
            if(action == 's') {
                game_status = action_show(board, width, height, mines_placed, mine_number);
            }
            else if(action == 'm') {
                action_mark(board, width, height);
            }
            else if(action == 'r') {
                game_status = 3;
            }
            else if(action == 'q') {
                game_status = 4;
            }
        }

        if(game_status < 3) {

            std::system("clear");
            unhide_all(board, width, height);
            print_board(board, width, height);
            if(game_status == 1) {
                std::cout << std::endl << "You won!" << std::endl;
            }
            else {
                std::cout << std::endl << "You lost!" << std::endl;
            }
            std::cout << std::endl << "Play again? Y/N" << std::endl;
            char action{};
            do {
                action = cin_char();
            } while((action != 'y') && (action != 'n'));
            if(action == 'y') game_status = 3;
        }

        delete[] board;
        board = nullptr;

    } while(game_status == 3);
    
    std::system("clear");

    return 0;
}

int action_show(char * board, int width, int height, bool & mines_placed, int mine_number) {
    /*
     * Shows (unhides) a selected tile
     * 
     * Returns 1 if the player has won, 2 if the player has lost, and 0 otherwise
     */

    int xloc{}, yloc{};
    do {
        std::cout << "X Location (1-" << width << ")" << std::endl;
        xloc = cin_int();
    } while(xloc < 1 || xloc > width);
    do {
        std::cout << "Y Location (1-" << height << ")" << std::endl;
        yloc = cin_int();
    } while(yloc < 1 || yloc > height);

    int index{ (yloc - 1)*width + xloc - 1 };

    // The mines aren't placed until the first time the player shows a tile
    if(!mines_placed) {
        place_mines(board, width, height, index, mine_number);
        compute_adjacent(board, width, height);
        mines_placed = true;
    }

    // The selected tile will only be shown if it isn't marked or already shown
    if(!(board[index] & MARKED_BIT) && (board[index] & HIDDEN_BIT)) {
        if(board[index] & MINE_BIT) {
            return 2;
        } else {
            unhide(board, width, height, xloc, yloc);
            return is_game_won(board, width, height);
        }
    }

    return 0;
}

void unhide(char * board, int width, int height, int xloc, int yloc) {
    /*
     * Helper function for action_show()
     */

    int index{(yloc - 1)*width + xloc - 1};

    // The tile will only unhidden if it is on the board and has not already been unhidden
    if((yloc > 0) && (yloc < height + 1) && (xloc > 0) && (xloc < width + 1) && (board[index] & HIDDEN_BIT)) {

        board[index] &= ~HIDDEN_BIT;
        board[index] &= ~MARKED_BIT;

        // unhiding adjacent tiles if the unhid tile has no adjacent mines
        if(!board[index]) {

            unhide( board, width, height, xloc  , yloc-1 ); // above
            unhide( board, width, height, xloc+1, yloc-1 ); // above & right
            unhide( board, width, height, xloc+1, yloc   ); // right
            unhide( board, width, height, xloc+1, yloc+1 ); // below & right
            unhide( board, width, height, xloc  , yloc+1 ); // below
            unhide( board, width, height, xloc-1, yloc+1 ); // below & left
            unhide( board, width, height, xloc-1, yloc   ); // left
            unhide( board, width, height, xloc-1, yloc-1 ); // above & left
        }
    }
}

void action_mark(char * board, int width, int height) {
    /*
     * Marks/unmarks a tile if the tile is still hidden
     */

    int xloc{}, yloc{};
    do {
        std::cout << "X Location (1-" << width << ")" << std::endl;
        xloc = cin_int();
    } while(xloc < 1 || xloc > width);
    do {
        std::cout << "Y Location (1-" << height << ")" << std::endl;
        yloc = cin_int();
    } while(yloc < 1 || yloc > height);
    int index{(yloc - 1)*width + xloc - 1};
    if(board[index] & HIDDEN_BIT) board[index] ^= MARKED_BIT;
}

int cin_int() {
    /*
     * Function to avoid infinite loop that occurs when std::cin tries to assign a non integer to an integer
     * 
     * Returns integer that player enters if the integer is a natural number, returns -1 otherwise
     */

    std::string input{};
    std::cin >> input;
    int num{};

    // std::string.length() returns unsigned long so static_cast<int>() is used to convert to int
    for(int i{}; i < static_cast<int>(input.length()); ++i) {

        if(input[i] < '0' || input[i] > '9') {
            return -1;
        }
        else {
            num *= 10;
            num += input[i] - '0';
        }
    }
    return num;
}

void start_game(char * & board, int & width, int & height, int & mine_number) {
    /*
     * Gets the game parameters from the player and creates the board
     */

    std::cout << "Welcome to Minesweeper!" << std::endl << std::endl;
    std::cout << "Choose board dimensions" << std::endl;
    do {
        std::cout << "Width (2-" << MAX_WIDTH << ")" << std::endl;
        width = cin_int();
    } while(width < 2 || width > MAX_WIDTH);
    do {
        std::cout << "Height (2-" << MAX_HEIGHT << ")" << std::endl;
        height = cin_int();
    } while (height < 2 || height > MAX_HEIGHT);
    do {
        std::cout << "Number of mines (1-" << width*height - 1 << ")" << std::endl;
        mine_number = cin_int();
    } while(mine_number < 1 || mine_number > width*height - 1);

    board = create_board(width, height);
}

char * create_board(int width, int height) {
    /*
     * Dynamically allocates the board array and initializes all the tiles to be hidden
     * 
     * Returns pointer to the array
     */

    char * board{new char[width*height]};
    for(int i{}; i < width*height; ++i) {
        board[i] = HIDDEN_BIT;
    }
    return board;
}

void place_mines(char * board, int width, int height, int safe_location, int mine_number) {
    /*
     * Randomly places mine_number mines on the field avoiding the safe_location
     */

    while(mine_number) {
        int try_position{};
        do {
            try_position = rand() % (width*height);
        } while((board[try_position] & MINE_BIT) || (try_position == safe_location));
        board[try_position] |= MINE_BIT;
        --mine_number;
    }
}

void print_board(char * board, int width, int height) {
    /*
     * Prints the current board to the console
     */

    std::cout << "   ";
    for(int i{}; i < width; ++i) std::cout << (i + 1)%10 << " ";
    std::cout << std::endl << std::endl;

    for(int y{}; y<height; ++y) {

        std::cout << (y + 1)%10 << "  ";
        for(int x{}; x<width; ++x) {

            int index{ y*width + x };
            if(board[index] & MARKED_BIT) {
                std::cout << 'M';
            }
            else if(board[index] & HIDDEN_BIT) {
                std::cout << 'X';
            }
            else if(board[index] & MINE_BIT) {
                std::cout << '!';
            }
            else if(board[index]) {
                std::cout << board[index] + 0;
            }
            else {
                std::cout << '-';
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

char cin_char() {
    /*
     * Returns the first char from user input
     */

    std::string input{};
    std::cin >> input;
    return tolower(input[0]);
}

void unhide_all(char * board, int width, int height) {
    /*
     * Unhides all the tiles on the board
     */

    for(int i{}; i<width*height; ++i) {
        board[i] &= ~MARKED_BIT;
        board[i] &= ~HIDDEN_BIT;
    }
}

bool is_game_won(char * board, int width, int height) {
    /*
     * Checks if the game has been won
     * 
     * Returns 1 if all non-mine tiles have been shown, otherwise returns 0
     */

    for(int i{}; i < width*height; ++i) {
        if((board[i] & HIDDEN_BIT) && !(board[i] & MINE_BIT)) {
            return false;
        }
    }
    return true;
}

void compute_adjacent(char * board, int width, int height) {
    /*
     * Computes the amount of adjacent tiles that contain mines for each tile
     */

    for(int index{}; index < width*height; ++index) {
        int xpos{index%width + 1};
        int ypos{index/width + 1};
        check_tile( board, index, width, height, xpos  , ypos-1 ); // checks tile above
        check_tile( board, index, width, height, xpos+1, ypos-1 ); // checks tile above & right
        check_tile( board, index, width, height, xpos+1, ypos   ); // checks tile right
        check_tile( board, index, width, height, xpos+1, ypos+1 ); // checks tile below & right
        check_tile( board, index, width, height, xpos  , ypos+1 ); // checks tile below
        check_tile( board, index, width, height, xpos-1, ypos+1 ); // checks tile below & left
        check_tile( board, index, width, height, xpos-1, ypos   ); // checks tile left
        check_tile( board, index, width, height, xpos-1, ypos-1 ); // checks tile above & left
    }
}

void check_tile(char * board, int index, int width, int height, int xpos, int ypos) {
    /*
     * Helper function for compute_adjacent()
     */

    if((ypos > 0) && (ypos < height + 1) && (xpos > 0) && (xpos < width + 1)) {
        int c{(ypos - 1)*width + xpos - 1};
        if(board[c] & MINE_BIT) {
            board[index] += 1;
        }
    }
}
