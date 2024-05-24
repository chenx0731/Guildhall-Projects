#pragma once
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Game/MapDefinition.hpp"
//#include 
#include <vector>

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
	Map(Game* owner, const char* name);
	~Map();
	
	void Render(const Camera& camera) const;
	void Update(float deltaSeconds);

	IntVec2	GetMapDimension() const { return m_dimensions; };
	Actor* GetBullet() const;
	Actor* GetActorByUID(ActorUID uid);
	Actor* GetClosetVisibleEnemy(Actor const* actor);

	RaycastResult3D RaycastAll(const Vec3& start, const Vec3& direction, float distance) const;
	RaycastResult3D RaycastAll(const Vec3& start, const Vec3& direction, float distance, Actor*& impactActor) const;

	// Spawn
	void SpawnPlayer(int playerIndex);
	Actor* SpawnActor(SpawnInfo spawnInfo);

	void DebugPossessNext();

	void MoveElevator(float goalCenter, float deltaSeconds);

	//void SpawnEntity

protected:
	
	void UpdateFromKeyboard();
	
	// tiles
	void RenderTiles() const;
	void PopulateTiles();
	void AddVertsForTile(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, int tileIndex) const;
	void AddVertsForElevator(std::vector<Vertex_PCUTBN>& verts, int tileIndex) const;

	bool IsPositionInBounds(Vec3 position, const float tolerance = 0.f) const;
	bool AreCoordsInBounds(int x, int y) const;
	const Tile* GetTile(int x, int y) const;

	IntVec2 GetCoordsOfActor(Actor* actor);
	IntVec2 GetCoordsOfPoint(Vec3 point) const;

	// collide actors
	void CollideActors();
	void CollideActors(Actor* actorA, Actor* actorB);
	void CollideActorsWithMap();
	void CollideActorWithMap(Actor* actor);
	void CollideActorWithTile(Actor* actor, int x, int y);

	// delete actor
	void DeleteDestroyedActors();


	// raycast
	RaycastResult3D RaycastWorldXY(const Vec3& start, const Vec3& direction, float distance) const;
	RaycastResult3D RaycastWorldZ(const Vec3& start, const Vec3& direction, float distance) const;
	RaycastResult3D RaycastWorldActors(const Vec3& start, const Vec3& direction, float distance) const;
	
	RaycastResult3D RaycastWorldActors(const Vec3& start, const Vec3& direction, float distance, Actor*& impactActor) const;

public:
	Vec3					m_sunDirection = Vec3(2.f, 1.f, -1.f);
	float					m_sunIntensity = 0.8f;
	float					m_ambientIntensity = 0.35f;
	Rgba8					m_ambientColor = Rgba8(100, 100, 255);
	ActorList				m_actors;
	float					m_level;
protected:
	Game*					m_game = nullptr;
	MapDefinition const*	m_mapDef = nullptr;

	Actor*					m_bullet;
	Actor*					m_elevator;
	IntVec2					m_elevatorOpen;
	IntVec2					m_elevatorClose;
	unsigned int			m_currentSalt = 1;

	std::vector<Tile>		m_tiles;
	IntVec2					m_dimensions = IntVec2(0, 0);

	std::vector<Vertex_PCUTBN>	m_vertexes;
	std::vector<unsigned int>	m_indexes;
	int						m_elevatorTile;

	Texture*				m_texture;
	Shader*					m_shader;
	VertexBuffer*			m_vertexBuffer;
	IndexBuffer*			m_indexBuffer;
};