#include "Game/BlockTemplate.hpp"
#include "Game/BlockDef.hpp"
#include "Engine/Core/FileUtils.hpp"


std::vector<BlockTemplate*> BlockTemplate::s_blockTemps;
BlockTemplate::BlockTemplate(std::string name, std::vector<BlockTemplateEntry> blocks)
{
	m_name = name;
	m_blocks = blocks;
}

BlockTemplate::BlockTemplate(std::string name, std::string filename)
{
	m_name = name;
	std::string result;
	FileReadToString(result, filename);
	Strings datas = SplitStringOnDelimiter(result, "data\":[[");
	std::string data = datas[1];
	Strings substrs = SplitStringOnDelimiter(data, "],[");
	for (int subIndex = 0; subIndex < (int)substrs.size(); subIndex++) {
		Strings details = SplitStringOnDelimiter(substrs[subIndex], "\"");
		IntVec3 temp, coord;
		temp.SetFromText(details[0].c_str());
		coord.x = temp.x;
		coord.y = temp.z;
		coord.z = temp.y;
		Rgba8 color;
		color.r = (unsigned char)stoi(details[1].substr(1, 2), nullptr, 16);
		color.g = (unsigned char)stoi(details[1].substr(3, 2), nullptr, 16);
		color.b = (unsigned char)stoi(details[1].substr(5, 2), nullptr, 16);
		color.a = 255;
		m_blocks.emplace_back(BlockTemplateEntry(BlockDef::GetBlockDefByColor(color), coord));
	}
}

BlockTemplate::~BlockTemplate()
{

}

BlockTemplate* BlockTemplate::GetTemplateByName(std::string name)
{
	for (int tempIndex = 0; tempIndex < (int)s_blockTemps.size(); tempIndex++) {
		BlockTemplate*& temp = s_blockTemps[tempIndex];
		if (temp != nullptr) {
			if (_strcmpi(temp->m_name.c_str(), name.c_str()) == 0) {
				return temp;
			}
		}
	}
	return nullptr;
}
