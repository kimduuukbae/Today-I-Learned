#pragma once

// A* 알고리즘에 사용될 노드 클래스
struct Node {
	Node(int tileX, int tileY, bool checkWall);

	int gCost{};	// Goal 경로 값
	int hCost{};	// Heuritic 값

	inline int fCost() const {	//Fitness 값
		return gCost + hCost;
	}
									
	std::pair<int, int> tilePos{};	// 노드의 타일(배열)상의 위치
	
	Node* parent{ nullptr };		// 바로 직전의 노드 좌표

	bool bIsWall{};					// 이 노드가 장애물 노드인지 확인
	//bool bIsVisited{};				// 들러본적이 있는지?
	bool operator==(const Node& rhs) const;
	inline bool operator<(const Node& rhs) const {
		return (fCost() >= rhs.fCost() && hCost > rhs.hCost);
	}
};

bool pathFinding(std::list<Node>& closeList);
std::pair<int, int> findPathWithAStar(int cX, int cY, int tX, int tY);
void endFindPath();
void getNeighbourList(std::vector<Node>* out, int cX, int cY);
int getDistance(int cX, int cY, int tX, int tY);

inline auto qBegin(std::priority_queue<Node>& queue) {
	return &(queue.top());
}

inline auto qEnd(std::priority_queue<Node>& queue) {
	return (&(queue.top()) + queue.size());
}