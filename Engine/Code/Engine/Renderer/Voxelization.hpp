#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>
struct Vec3;
struct Vertex_PCUTBN;
void VoxelizeModel(const std::vector<Vertex_PCUTBN>& vertexes, const std::vector<unsigned int>& indexes, std::vector<Vec3>& centers);