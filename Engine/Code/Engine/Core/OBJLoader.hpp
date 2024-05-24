#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>
struct Mat44;
struct Vertex_PCUTBN;

void LoadOBJByFileName(const char* filename, Mat44 transformMat, std::vector<Vertex_PCUTBN>& outVertexes, std::vector<unsigned int>& outIndexes);