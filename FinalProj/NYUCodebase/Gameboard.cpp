
#include "GameBoard.h"
#include <vector>

using namespace std;


enum PlayerMode { ONE, TWO };


//	GameMode mode;
//	std::vector<std::vector<int>> Board;


bool compareColor(TileType a, TileType b) {
	if (a == RED || a == RED_CLEAR) return b == RED || b == RED_CLEAR;
	else if (a == BLUE || a == BLUE_CLEAR) return b == BLUE || b == BLUE_CLEAR;

	return false;
}

Gameboard::Gameboard() {}

void Gameboard::createBoard(PlayerMode mode) {
	if (mode == ONE)
		Board = vector<vector<TileType>>(4);
	else if (mode == TWO)
		Board = vector<vector<TileType>>(8);
	else
		Board = vector<vector<TileType>>(1); // Error

	for (vector<TileType>& slot : Board)
		slot = vector<TileType>(5, EMPTY);
}

int Gameboard::insert(int columnNum, TileType tileType) {
	for (unsigned int i = 0; i < Board[columnNum].size(); i++) {
		if (Board[columnNum][i] != EMPTY)	;

		else {
			Board[columnNum][i] = tileType;
			return 0;
		}
	}

	for (unsigned int i = 0; i < Board[columnNum].size(); i++)
		Board[columnNum][i] = EMPTY;

	return 10;
}

void Gameboard::clear() {
	Board.clear();
}

void Gameboard::fall() {
	for (int i = 0; i < Board.size(); i++) {
		for (int j = 1; j < Board[0].size(); j++) {
			if ((Board[i][j] == RED || Board[i][j] == BLUE || Board[i][j] == GREY) && Board[i][j - 1] == EMPTY) {

				//Falling condition met, start finding the lowest empty tile
				for (int k = 2;; k++) {
					//Start replacing the tiles
					if (j - k < 0 || Board[i][j - k] != EMPTY) {
						Board[i][j - k + 1] = Board[i][j];
						Board[i][j] = EMPTY;
						break;
					}
				}
			}
		}
	}
}


int Gameboard::clearMatch() {
	int output = 0;

	for (int i = 0; i < Board.size(); i++) {
		for (int j = 0; j < Board[0].size(); j++) {
			if (Board[i][j] == RED_CLEAR || Board[i][j] == BLUE_CLEAR) {
				Board[i][j] = EMPTY;
				if (j - 1 >= 0 && Board[i][j - 1] == GREY) Board[i][j - 1] = EMPTY;

				output += output / 3;
				output++;
			}
		}
	}

	return output;
}

int Gameboard::scoreMatch() {
	bool clear = false;

	for (int i = 0; i < Board.size(); i++) {
		for (int j = 0; j < Board[0].size(); j++) {
			if (Board[i][j] != EMPTY && Board[i][j] != GREY) {
				int count = 1;
				bool left = false, right = false, up = false, down = false;

				//Check the matches
				if (left = i != 0 && compareColor(Board[i - 1][j], Board[i][j])) {
					count++;
				}
				if (right = i + 1 < Board.size() && compareColor(Board[i + 1][j], Board[i][j])) {
					count++;
				}
				if (down = j != 0 && compareColor(Board[i][j - 1], Board[i][j])) {
					count++;
				}
				if (up = j + 1 < Board[0].size() && compareColor(Board[i][j + 1], Board[i][j])) {
					count++;
				}
				
				//Start replacing tiles
				if (count >= 3) {
					clear = true;

					TileType clearType;
					if (Board[i][j] == BLUE || Board[i][j] == BLUE_CLEAR) clearType = BLUE_CLEAR;
					else if (Board[i][j] == RED || Board[i][j] == RED_CLEAR) clearType = RED_CLEAR;

					Board[i][j] = clearType;
					if (left) Board[i - 1][j] = clearType;
					if (right) Board[i + 1][j] = clearType;
					if (down) Board[i][j - 1] = clearType;
					if (up) Board[i][j + 1] = clearType;
				}
			}
		}
	}

	if (clear) {
		int output = clearMatch();
		fall();
		return output;
	}
	else {
		fall();
		return 0;
	}
}
