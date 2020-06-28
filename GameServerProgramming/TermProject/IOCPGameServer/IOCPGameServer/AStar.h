#pragma once

// A* �˰��� ���� ��� Ŭ����
struct Node {
	Node(int tileX, int tileY, bool checkWall);

	int gCost{};	// Goal ��� ��
	int hCost{};	// Heuritic ��

	inline int fCost() const {	//Fitness ��
		return gCost + hCost;
	}
									
	std::pair<int, int> tilePos{};	// ����� Ÿ��(�迭)���� ��ġ
	
	Node* parent{ nullptr };		// �ٷ� ������ ��� ��ǥ

	bool bIsWall{};					// �� ��尡 ��ֹ� ������� Ȯ��
	//bool bIsVisited{};				// �鷯������ �ִ���?
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