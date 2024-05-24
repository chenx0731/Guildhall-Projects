#pragma once
#include "Game/Tile.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <vector>
#include <string>

class TileDefinition
{
public: 
	TileDefinition(XmlElement& tileDefElement);
	~TileDefinition();
	static void InitializeTileDefs(const char* docFilePath);
	static TileDefinition const* GetTileDefByName(std::string const& tileDefName);
	static TileDefinition const* GetTileDefBySymbol(char const& tileDefSymbol);
	static void ClearDefinitions();
public:
	static std::vector<TileDefinition*> s_TileDefs;

	char			m_symbol = 0;
	std::string		m_name;
	bool			m_isBlocked = true;
};
