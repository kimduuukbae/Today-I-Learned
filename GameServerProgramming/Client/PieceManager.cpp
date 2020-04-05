#include "stdafx.h"
#include "PieceManager.h"

static PieceManager manager;

PieceManager& PieceManager::getInstance(){
	return manager;
}

Piece& PieceManager::getPiece(int id){
	return pieces[id];
}

void PieceManager::initPiece(const E_CHESS_TYPE& type, int id, int x, int y){
	pieces[id] = Piece{type, x, y};
}

void PieceManager::movePiece(int id, int x, int y) {
	pieces[id].Move(x, y);
}

bool PieceManager::find(int id){
	return (pieces.find(id) != pieces.end());
}

std::pair<std::map<int, Piece>::iterator, std::map<int, Piece>::iterator> PieceManager::getAllPiece(){
	return std::make_pair(pieces.begin(), pieces.end());
}

void PieceManager::setHeroID(int id){
	heroID = id;
}
