#pragma once
#include "Packet.h"

class Board {
public:
	void initialize(int x, int y);
	void destroy();
	void setActorInBoard(const E_CHESS_TYPE& type, int x, int y);
	E_CHESS_TYPE getActorTypeInBoard(int x, int y);
private:
	char** board;
	int boardXSize;
	int boardYSize;
};