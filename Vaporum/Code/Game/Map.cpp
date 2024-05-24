#define UNUSED(x) (void)(x);
#include "Game/Map.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Unit.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Input/InputSystem.hpp"

IntVec2 HexNeighbors[6] = {
	IntVec2(1, 0),
	IntVec2(0, 1),
	IntVec2(-1, 0),
	IntVec2(0, -1),
	IntVec2(1, -1),
	IntVec2(-1, 1),
};

Map::Map(GamePlaying* owner, const char* name) : m_game(owner)
{
	m_mapDef = MapDefinition::GetMapDef(name);
	if (m_mapDef == nullptr) {
		ERROR_AND_DIE(Stringf("Error: Map: %s doesn't exsit", name));
	}

	for (int tileIndex = 0; tileIndex < m_mapDef->m_tiles.size(); tileIndex++) {
		Tile* const& tile = m_mapDef->m_tiles[tileIndex];
		if (!tile->IsBlocked() && m_mapDef->m_worldBounds.IsPointInside(tile->m_center))
			AddVertsForHexgon3DFrame(m_vertexes, m_indexes, tile->m_radius, tile->m_center, 0.05f, Vec3(0.f, 0.f, -1.f));
		else if (tile->IsBlocked())
			AddVertsForHexgon3D(m_vertexes, m_indexes, tile->m_radius - 0.05f, tile->m_center, Vec3(0.f, 0.f, -1.f), Rgba8::BLACK);
		//AddVertsForHexgon3D(m_vertexes, m_indexes, tile->m_radius, tile->m_center, Vec3(0.f, 0.f, -1.f));
	}
	m_vertexBuffer = g_theRenderer->CreateVertexBuffer((int)m_vertexes.size() * sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer((int)m_indexes.size() * sizeof(unsigned int));
	
	for (int tileIndex = 0; tileIndex < (int)m_mapDef->m_tiles.size(); tileIndex++) {
		Node node;
		node.m_coords = m_mapDef->m_tiles[tileIndex]->m_tileCoords;
		m_defaultNodes[node.m_coords] = node;
		//m_distanceField[node.m_coords] = node;
	}
	//m_defaultNodes;
}

Map::~Map()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

void Map::Render(const Camera& camera) const
{
	g_theRenderer->BeginCamera(camera);

	g_theRenderer->DrawVertexIndexArray((int)m_vertexes.size(), (int)m_indexes.size(), m_vertexes.data(), m_indexes.data(), m_vertexBuffer, m_indexBuffer);

	RenderField();
	RenderPath();
	g_theRenderer->EndCamera(camera);
	//DrawCircle()
}

void Map::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	m_game->m_currentPlayer->m_position.z = GetClamped(m_game->m_currentPlayer->m_position.z, m_mapDef->m_worldBounds.m_mins.z, m_mapDef->m_worldBounds.m_maxs.z);
		//m_mapDef->m_worldBounds.GetNearestPoint(m_game->m_player->m_position);
	//m_game->m_player->m_camera.m_position = m_game->m_player->m_position;
	float z = m_game->m_currentPlayer->m_position.z;
	Vec3 forward = m_game->m_currentPlayer->m_camera.m_orientation.GetForwardVector();
	if (forward.z != 0.f) {
		Vec3 center = m_game->m_currentPlayer->m_position + forward * (-z / forward.z);
		//center = m_mapDef->m_worldBounds.
		Vec2 centerXY = center.GetVec2XY();
		centerXY = m_mapDef->m_worldBounds.GetAABB2XY().GetNearestPoint(centerXY);
		m_game->m_currentPlayer->m_position.x = centerXY.x - forward.x * (-z / forward.z);
		m_game->m_currentPlayer->m_position.y = centerXY.y - forward.y * (-z / forward.z);
	}

}

void Map::GenerateDistanceField(IntVec2 start, int range)
{
	m_pQueue = std::priority_queue<Node, std::vector<Node>, std::greater<Node>>();
	ResetDefaultField();

	Node& node = m_defaultNodes[start];
	node.m_coords = start;
	node.m_distance = 0;
	node.m_isOpened = true;
	m_pQueue.push(node);
	while (!m_pQueue.empty()) {
		Node pop = m_pQueue.top();
		if (pop.m_distance >= range)
			break;
		for (int nbIndex = 0; nbIndex < 6; nbIndex++) {
			IntVec2 nbCoords = pop.m_coords + HexNeighbors[nbIndex];
			// 1. isValidTile 2. isNotOpened&Closed 3. 
			if (GetTileByCoords(nbCoords)) {
				Node& nbNode = m_defaultNodes[nbCoords];
				if (!nbNode.m_isOpened && !nbNode.m_isClosed) {
					nbNode.m_distance = pop.m_distance + 1;
					nbNode.m_isOpened = true;
					m_pQueue.push(nbNode);
				}
			}
		}
		Node& realPop = m_defaultNodes[pop.m_coords];
		realPop.m_isClosed = true;
		m_pQueue.pop();
	}
}

void Map::GeneratePath(Vec3 pos)
{
	m_distancePath.clear();
	IntVec2 coords = GetCoordByPosition(pos);
	Node now = m_defaultNodes[coords];
	m_distancePath.push_back(now);
	while (now.m_distance != 9999999 && now.m_distance != 0) {
		for (int nbIndex = 0; nbIndex < 6; nbIndex++) {
			IntVec2 nbcoords = now.m_coords + HexNeighbors[nbIndex];
			Node nb = m_defaultNodes[nbcoords];
			if (nb.m_distance < now.m_distance) {
				m_distancePath.push_back(nb);
				now = nb;
				break;
			}
		}
	}
}

void Map::ResetDefaultField()
{
	for (auto it = m_defaultNodes.begin(); it != m_defaultNodes.end(); it++) {
		Node& node = it->second;
		node.m_distance = 9999999;
		node.m_isOpened = false;
		node.m_isClosed = false;
	}
	m_distancePath.clear();
}

void Map::RenderPath() const
{
	//m_defaultNodes
	if (m_distancePath.size() == 1)
		return;
	std::vector<Vertex_PCU> path;
	//Node node = m_defaultNodes[coords];
	for (int i = 0; i < (int)m_distancePath.size(); i++) {
		Vec3 pos = GetPostionByCoords(m_distancePath[i].m_coords);
		AddVertsForHexgon3D(path, 0.52f, pos, Rgba8(255, 255, 255, 127));
		AddVertsForHexgon3DFrame(path, 0.577f, pos, 0.1f, Rgba8(240, 240, 240));
	}
	g_theRenderer->DrawVertexArray((int)path.size(), path.data());
}

void Map::RenderField() const
{
	std::vector<Vertex_PCU> field;
	for (auto it = m_defaultNodes.begin(); it != m_defaultNodes.end(); it++) {
		const Node& node = it->second;
		if (node.m_distance < 9999999) {
			Vec3 center = GetPostionByCoords(it->first);

			AddVertsForHexgon3D(field, 0.52f, center, Rgba8(200, 200, 200, 127));
			AddVertsForHexgon3DFrame(field, 0.577f, center, 0.1f, Rgba8(220, 220, 220));
		}
	}
	g_theRenderer->DrawVertexArray((int)field.size(), field.data());
}

RaycastResult3D Map::RaycastWorldZ(const Vec3& start, const Vec3& direction) const
{
	RaycastResult3D result;
	float level = 0.f;
	if (direction.z == level)
		return result;
	if (direction.z > level) {
		float t = (1.f + level - start.z) / direction.z;
		if (t > 0.f) {
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
			result.m_impactPos = start + t * direction;
			result.m_impactDis = GetDistance3D(result.m_impactPos, start);
		}
	}
	else {
		float t = -start.z / direction.z;
		if (t > 0.f) {
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
			result.m_impactPos = start + t * direction;
			result.m_impactDis = GetDistance3D(result.m_impactPos, start);
		}
	}
	return result;

}

IntVec2 Map::GetCoordByPosition(const Vec3& position) const
{
	for (int tileIndex = 0; tileIndex < (int)m_mapDef->m_tiles.size(); tileIndex++) {
		Tile* const& tile = m_mapDef->m_tiles[tileIndex];
		if (tile->IsPointInside(position) && !tile->IsBlocked()) {
			return tile->m_tileCoords;
			//AddVertsForHexgon3DFrame(hitTile, hitIndex, tile->m_radius - 0.2f, tile->m_center, 0.02f, Vec3(0.f, 0.f, -1.f));
			//break;
		}
	}
	return IntVec2(-999, -999);
}

Vec3 Map::GetPostionByCoords(const IntVec2& coord) const
{
	Vec3 pos = Vec3(0.866f * float(coord.x), 0.5f * float(coord.x) + float(coord.y), 0.01f) * 0.577f * 1.732f;
	pos.z = 0.01f;
	return pos;
}

Tile* Map::GetTileByPosition(const Vec3& position) const
{
	for (int tileIndex = 0; tileIndex < (int)m_mapDef->m_tiles.size(); tileIndex++) {
		Tile* const& tile = m_mapDef->m_tiles[tileIndex];
		if (tile->IsPointInside(position) && !tile->IsBlocked()) {
			return tile;
			//AddVertsForHexgon3DFrame(hitTile, hitIndex, tile->m_radius - 0.2f, tile->m_center, 0.02f, Vec3(0.f, 0.f, -1.f));
			//break;
		}
	}
	return nullptr;
}


Tile* Map::GetTileByCoords(const IntVec2& coord) const
{
	for (int tileIndex = 0; tileIndex < (int)m_mapDef->m_tiles.size(); tileIndex++) {
		Tile* const& tile = m_mapDef->m_tiles[tileIndex];
		if (tile->m_tileCoords == coord && !tile->IsBlocked()) {
			return tile;
			//AddVertsForHexgon3DFrame(hitTile, hitIndex, tile->m_radius - 0.2f, tile->m_center, 0.02f, Vec3(0.f, 0.f, -1.f));
			//break;
		}
	}
	return nullptr;
}

void Map::SetUnitsByPlayer(std::vector<Unit*>& units, int playerIndex)
{
	if (playerIndex == 1) {
		for (int unitIndex = 0; unitIndex < (int)m_mapDef->m_unitsP1.size(); unitIndex++) {
			Unit* unit = new Unit();
			unit->m_unitDef = m_mapDef->m_unitsP1[unitIndex]->m_unitDef;
			unit->m_position = m_mapDef->m_unitsP1[unitIndex]->m_position;
			unit->m_orientation = m_mapDef->m_unitsP1[unitIndex]->m_orientation;
			unit->m_radius = m_mapDef->m_unitsP1[unitIndex]->m_radius;
			unit->m_color = m_mapDef->m_unitsP1[unitIndex]->m_color;
			unit->m_state = m_mapDef->m_unitsP1[unitIndex]->m_state;
			unit->m_currentHealth = m_mapDef->m_unitsP1[unitIndex]->m_currentHealth;
			unit->m_coords = m_mapDef->m_unitsP1[unitIndex]->m_coords;
			units.push_back(unit);
		}
	}
	else if (playerIndex == 2) {
		for (int unitIndex = 0; unitIndex < (int)m_mapDef->m_unitsP2.size(); unitIndex++) {
			Unit* unit = new Unit();
			unit->m_unitDef = m_mapDef->m_unitsP2[unitIndex]->m_unitDef;
			unit->m_position = m_mapDef->m_unitsP2[unitIndex]->m_position;
			unit->m_orientation = m_mapDef->m_unitsP2[unitIndex]->m_orientation;
			unit->m_radius = m_mapDef->m_unitsP2[unitIndex]->m_radius;
			unit->m_color = m_mapDef->m_unitsP2[unitIndex]->m_color;
			unit->m_state = m_mapDef->m_unitsP2[unitIndex]->m_state;
			unit->m_currentHealth = m_mapDef->m_unitsP2[unitIndex]->m_currentHealth;
			unit->m_coords = m_mapDef->m_unitsP2[unitIndex]->m_coords;
			units.push_back(unit);
		}
	}
}
