#pragma once
#include <map>
#include "Packet.h"
#include "Piece.h"


class PieceManager {
public:
	static PieceManager& getInstance();
	Piece& getPiece(int id);
	void initPiece(const E_CHESS_TYPE& type, int id, int x, int y);
	void movePiece(int id, int x, int y);
	bool find(int id);
	std::pair<std::map<int, Piece>::iterator, std::map<int, Piece>::iterator> getAllPiece();
	inline size_t getSize() {
		return pieces.size();
	}
	void setHeroID(int id);
	inline int getHeroID() {
		return heroID;
	}
private:
	std::map<int, Piece> pieces;
	int heroID;
};