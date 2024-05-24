#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include <map>
#include <string>

class Map;
class Player;
class GamePlaying;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Actor;
class ActorUID;
struct Vec2;
struct RaycastResult3D;
struct SpawnInfo;
typedef std::vector<Actor*> ActorList;

class World
{
public:
	World(GamePlaying* game, std::string name);
	~World();
	void Update(float deltaseconds);
	void Render(const Camera& camera) const;

	void RemovePlayerFromMap(int playerIndex);
	void AddPlayerToMap(int playerIndex, Map* map);

	Map* GetMapByLevel(float level);

	Mat44 GetDogModelMatrix() const;

	//IntVec2	GetMapDimension() const { return m_dimensions; };
	//Actor* GetBullet() const;


private: 
	void UpdateFromKeyboard(float deltaSeconds);



public:
	GamePlaying*				m_game = nullptr;
	std::string					m_name;
	Map*						m_currentMap;
	int							m_currentMapIndex = 0;
	std::string					m_currentMapName;
	Strings						m_mapNames;
	float						m_elevatorGoal = 0.5f;
	std::map<std::string, Map*> m_maps;

	std::vector<Vertex_PCUTBN>	m_dogVerts;
	std::vector<unsigned int>	m_dogIndexes;

	VertexBuffer*				m_dogVertexBuffer;
	IndexBuffer*				m_dogIndexBuffer;

	Shader*						m_shader;

	Vec3						m_dogPosition = Vec3(10.f, 10.f, 0.f);
	EulerAngles					m_dogOrientation;

	Actor*						m_dogActor = nullptr;

	bool						m_isRiding;
	ActorList					m_actors;
};