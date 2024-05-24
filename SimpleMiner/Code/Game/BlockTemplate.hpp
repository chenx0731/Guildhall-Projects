#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec3.hpp"
#include <vector>
#include <string>

class BlockDef;

struct BlockTemplateEntry
{
public:
	BlockTemplateEntry(unsigned char def, IntVec3 coords){
		m_blockDef = def;
		m_coords = coords;
	}
	~BlockTemplateEntry(){}
public:
	unsigned char m_blockDef;
	IntVec3 m_coords;
};

class BlockTemplate
{
public:
	BlockTemplate(std::string name, std::vector<BlockTemplateEntry> blocks);
	BlockTemplate(std::string name, std::string filename);
	~BlockTemplate();

	static BlockTemplate* GetTemplateByName(std::string name);
	
public:
	static std::vector<BlockTemplate*> s_blockTemps;
	std::string						m_name;
	std::vector<BlockTemplateEntry> m_blocks;

};