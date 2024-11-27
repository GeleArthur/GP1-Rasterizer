#pragma once
#include <string>
#include <vector>

namespace dae
{
	struct Vertex;
}

namespace dae::Utils
{
	void BetterParseObj(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
}
