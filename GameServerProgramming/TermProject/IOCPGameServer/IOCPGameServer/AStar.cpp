#include "stdafx.h"
#include "AStar.h"
#include <algorithm>
Node::Node(int tileX, int tileY, bool checkWall) : 
	tilePos{tileX, tileY},
	bIsWall{checkWall}
{}

bool Node::operator==(const Node& rhs) const{
	return (tilePos.first == rhs.tilePos.first &&
		tilePos.second == rhs.tilePos.second);
}

bool pathFinding(std::list<Node>& closeList,
int cX, int cY, int tX, int tY) {
	// 현재 노드와 목표 노드
	Node goalNode{ tX, tY, false };
	std::priority_queue<Node> openList{};// Open Node List
	std::unordered_set<int> searchSet{};
	searchSet.reserve(5000);
	openList.emplace(cX, cY, false);	//startNode
	constexpr int gCostPlane{ 50 };
	while (openList.size() > 0) {
		// 오픈리스트의 첫 번째 노드는 현재노드
		Node curNode{ openList.top() };
		openList.pop();

		closeList.push_back(curNode);
		searchSet.insert(curNode.tilePos.first * 1000 + curNode.tilePos.second);
		// 변경된 현재노드가 Target 노드랑 같으면 끝
		if (curNode == goalNode) 
			return true;
		
		// 이웃노드 openList 추가, goal, heuritic 추가
		std::vector<Node> neighbourList{};
		getNeighbourList(&neighbourList, curNode.tilePos.first, curNode.tilePos.second);
		auto end2 = neighbourList.end();
		for (auto it = neighbourList.begin(); it != end2; ++it) {
			if (!it->bIsWall && !searchSet.count(it->tilePos.first * 1000 + it->tilePos.second)) {
				auto [nX, nY] {it->tilePos};
				int movCost{curNode.gCost + 1};
				if (movCost > gCostPlane)
					continue;

				if (openList.size()) {
					auto qend{ qEnd(openList) };
					if (std::find(qBegin(openList), qend, *it) == qend) {
						it->gCost = movCost;
						it->hCost = getDistance(nX, nY, tX, tY);
						it->parent = &closeList.back();
						openList.push(*it);
					}
				}
				else {
					it->gCost = movCost;
					it->hCost = getDistance(nX, nY, tX, tY);
					it->parent = &closeList.back();
					openList.push(*it);
				}
			}
		}
	}
	return false;
}

std::pair<int, int> findPathWithAStar(int cX, int cY, int tX, int tY) {
	if (cX == tX && cY == tY)
		return { cX, cY };

	std::list<Node> closeList{};// Close Node List
	if (!pathFinding(closeList, cX, cY, tX, tY))
		return { cX, cY };

	std::vector<std::pair<int, int>> v{};
	v.reserve(80);
	Node* start{ &closeList.back() };
	while (start) {
		v.push_back(start->tilePos);
		start = start->parent;
	}
	
	return v[v.size() - 2];
}

void getNeighbourList(std::vector<Node>* out, int cX, int cY){
	if (cX > 0) 
		out->emplace_back(cX - 1, cY, mapTile[cX - 1][cY]);
	if (cX < WORLD_WIDTH - 1)
		out->emplace_back(cX + 1, cY, mapTile[cX + 1][cY]);
	if (cY > 0)
		out->emplace_back(cX, cY - 1, mapTile[cX][cY - 1]);
	if (cY < WORLD_HEIGHT - 1)
		out->emplace_back(cX, cY + 1, mapTile[cX][cY + 1]);

}

// manhattanDistance 임!
int getDistance(int cX, int cY, int tX, int tY){
	return (std::abs(tX - cX) + std::abs(tY - cY));
}
