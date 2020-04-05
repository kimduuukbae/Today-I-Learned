#include "stdafx.h"
#include "Piece.h"

Piece::Piece(const E_CHESS_TYPE& type, int x, int y){
	chessType = type;
	xPos = x;
	yPos = y;
}

void Piece::Move(int x, int y){
	xPos = x;
	yPos = y;
}
