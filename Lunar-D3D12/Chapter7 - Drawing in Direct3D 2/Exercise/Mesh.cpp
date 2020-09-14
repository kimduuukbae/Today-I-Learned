#include "stdafx.h"
#include "Mesh.h"

std::pair<std::vector<Vertex>, std::vector<int>> loadVertexFromFile(const std::string_view& fileName) {
	using namespace std::literals;
	
	std::ifstream ifs{ fileName.data() };
	// VertexCount
	std::istream_iterator<std::string> start{ ifs };
	std::istream_iterator<std::string> end{};

	++start;
	int VertexCount{ std::stoi(*start) };
	++start;
	++start;
	int TriangleCount{ std::stoi(*start) };
	int IndexCount{ TriangleCount * 3 };
	
	std::string ignore{};
	ifs >> ignore >> ignore >> ignore >> ignore;

	Vertex v{};
	std::vector<Vertex> vertex{};
	std::vector<int> index(IndexCount);
	vertex.reserve(VertexCount);

	//Vertex
	for (int i = 0; i < VertexCount; ++i) {
		ifs >> v.pos.x >> v.pos.y >> v.pos.z >> ignore >> ignore >> ignore;
		vertex.push_back(v);
	}
	
	ifs >> ignore >> ignore >> ignore;

	//Triangle
	for (int i = 0; i < TriangleCount; ++i) 
		ifs >> index[(i * 3)] >> index[(i * 3) + 1] >> index[(i * 3) + 2];

	return std::make_pair(std::move(vertex), std::move(index));
}