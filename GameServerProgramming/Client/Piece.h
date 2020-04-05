#pragma once
#include "Packet.h"
class Piece {
public:
	Piece() = default;
	Piece(const E_CHESS_TYPE& type, int x, int y);
	void Move(int x, int y);
	inline Position getPosition() {
		return Position{ xPos, yPos };
	}
private:
	E_CHESS_TYPE chessType;
	int xPos;
	int yPos;
};
