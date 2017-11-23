
#include "GameBoard.h"
#include <vector>

using namespace std;

//	GameMode mode;
//	std::vector<std::vector<int>> Board;


bool compareColor(TileType a, TileType b) {
	if (a == RED || a == RED_CLEAR) return b == RED || b == RED_CLEAR;
	else if (a == BLUE || a == BLUE_CLEAR) return b == BLUE || b == BLUE_CLEAR;

	return false;
}

Gameboard::Gameboard() {}

void Gameboard::createBoard(GameMode mode) {
	if (mode == SINGLE_PLAYER)
		Board = vector<vector<TileType>>(4);
	else if (mode == TWO_PLAYER)
		Board = vector<vector<TileType>>(8);
	else
		Board = vector<vector<TileType>>(1); // Error

	for (vector<TileType>& slot : Board)
		slot = vector<TileType>(7, EMPTY);
}

bool Gameboard::insert(int columnNum, TileType tileType) {
	for (int i = 0; i < Board[columnNum].size; i++) {
		if (Board[columnNum][i] != EMPTY)	;

		else {
			Board[columnNum][i] = tileType;
			return true;
		}
	}

	return false;
}

void Gameboard::clear() {
	Board.clear();
}

void Gameboard::fall() {
	for (int i = 0; i < Board.size(); i++) {
		for (int j = 1; j < Board[0].size(); j++) {
			if (Board[i][j] == RED || Board[i][j] == BLUE || Board[i][j - 1] == EMPTY) {

				//Falling condition met, start finding the lowest empty tile
				for (int k = 2; k <= j; k++) {
					//Start replacing the tiles
					if (j - k < 0 || Board[i][j - k] != EMPTY) {
						Board[i][j - k + 1] = Board[i][j];
						Board[i][j] = EMPTY;
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
				bool left, right, up, down;

				//Check the matches
				if (left =	(i - 1 >= 0					&& compareColor(Board[i - 1][j], Board[i][j]))) count++;
				if (right = (i + 1 < Board.size()		&& compareColor(Board[i + 1][j], Board[i][j]))) count++;
				if (down =	(j - 1 >= 0					&& compareColor(Board[i][j - 1], Board[i][j]))) count++;
				if (up =	(j + 1 >= Board[0].size()	&& compareColor(Board[i][j + 1], Board[i][j]))) count++;

				//Start replacing tiles
				if (count >= 3) {
					clear = true;

					TileType clearType;
					if (Board[i][j] == BLUE) clearType = Board[i][j] = BLUE_CLEAR;
					else if (Board[i][j] == RED) clearType = Board[i][j] = RED_CLEAR;

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
		return 0;
	}
}
