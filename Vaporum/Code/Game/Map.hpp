#pragma once
#include "Game/Tile.hpp"
#include "Game/MapDefinition.hpp"
#include <vector>
#include <queue>
#include <map>

class GamePlaying;
struct RaycastResult2D;
struct RaycastResult3D;
struct Vertex_PCUTBN;
class TileHeatMap;
class Player;
class Actor;
class Texture;
class Shader;
class VertexBuffer;
class IndexBuffer;
class ActorUID;
class Camera;
class Unit;
typedef std::vector<Actor*> ActorList;

enum HeatMapType
{
	HEAT_MAP_NULL = -1,

	HEAT_MAP_DISTANCE,
	HEAT_MAP_SOLID,
	HEAT_MAP_OPAQUE,

	HEAT_MAP_NUM
};

class Map
{
public:
	Map(GamePlaying* owner, const char* name);
	~Map();
	
	void Render(const Camera& camera) const;
	void Update(float deltaSeconds);

	void GenerateDistanceField(IntVec2 start, int range);
	void GeneratePath(Vec3 pos);
	void ResetDefaultField();

	void RenderPath() const;
	void RenderField() const;
public:
	RaycastResult3D RaycastWorldZ(const Vec3& start, const Vec3& direction) const;
	IntVec2 GetCoordByPosition(const Vec3& position) const;
	Vec3 GetPostionByCoords(const IntVec2& coord) const;
	Tile* GetTileByPosition(const Vec3& position) const;
	Tile* GetTileByCoords(const IntVec2& coord) const;

	void SetUnitsByPlayer(std::vector<Unit*> &units, int playerIndex = 0);
public:
	GamePlaying*			m_game = nullptr;
	MapDefinition const*	m_mapDef = nullptr;
	std::map<IntVec2, Node> m_defaultNodes;
protected:
	//std::vector<Tile>		m_tiles;
	IntVec2					m_dimensions = IntVec2(0, 0);
	VertexBuffer*			m_vertexBuffer = nullptr;
	IndexBuffer*			m_indexBuffer = nullptr;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;

	std::vector<Node> m_distancePath;
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> m_pQueue;
};