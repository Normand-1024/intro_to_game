
#pragma once

#include <vector>

enum PlayerMode;
enum TileType { EMPTY, RED, BLUE, GREY, RED_CLEAR, BLUE_CLEAR };

bool compareColor(TileType a, TileType b);

struct Gameboard {
    
	Gameboard();
    
	// Create the game board given the number of players, the board is 4 * 7 in single player, and 8 * 7 in two player
	void createBoard(PlayerMode mode);

	// Insert a tile into the game board, return False if the column being inserted into is full
	int insert(int columnNum, TileType tileType);

	// Clear the board
	void clear();

	// Let the tiles fall to the bottom empty tile
	void fall();

	// Clear any RED_CLEAR and BLUE_CLEAR tiles in the board, any GREY tile directly below a CLEAR tile will also be erased, return the score
	int clearMatch();

	// Check any adjecent three or more tiles and delete them, return the score earned by matching the tiles
	int scoreMatch();

	std::vector<std::vector<TileType>> Board;
};
