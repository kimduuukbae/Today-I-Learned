#include "stdafx.h"
#include "Board.h"
#include <string.h>
void Board::initialize(int x, int y){

	board = new char*[y];
	for (int i = 0; i < y; ++i) {
		board[i] = new char[x];
		memset(board[i], 0, sizeof(char) * x);
	}
	boardXSize = x;
	boardYSize = y;
	
	//memset(board, 0, x * y);
}

void Board::destroy(){
	
	for (int i = 0; i < boardYSize; ++i)
		delete[] board[i];
	delete[] board;
}

void Board::setActorInBoard(const E_CHESS_TYPE& type, int x, int y){
	board[y][x] = static_cast<char>(type);
}

E_CHESS_TYPE Board::getActorTypeInBoard(int x, int y){
	return static_cast<E_CHESS_TYPE>(board[y][x]);
}
