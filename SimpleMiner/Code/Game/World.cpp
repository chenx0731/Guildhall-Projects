#define UNUSED(x) (void)(x);
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Chunk.hpp"
#include "Game/BlockDef.hpp"
#include "Game/BlockTemplate.hpp"
#include "Game/ChunkGenerateJob.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/JobSystem.hpp"

World::World(Game* owner)
{
	UNUSED(owner);
	m_game = owner;
	m_activitionRange = g_gameConfigBlackboard.GetValue("chunkActivationDistance", 250.f);
	m_nightSkyColor				= g_gameConfigBlackboard.GetValue("nightSkyColor", Rgba8::WHITE);
	m_daySkyColor				= g_gameConfigBlackboard.GetValue("daySkyColor", Rgba8::WHITE);
	m_nightOutdoorLightColor	= g_gameConfigBlackboard.GetValue("nightOutdoorLightColor", Rgba8::WHITE);
	m_dayOutdoorLightColor		= g_gameConfigBlackboard.GetValue("dayOutdoorLightColor", Rgba8::WHITE);
	m_indoorLightColor			= g_gameConfigBlackboard.GetValue("indoorLightColor", Rgba8::WHITE);
	m_maxChunksRadiusX = 1 + int(m_activitionRange) / LOCAL_X_SIZE;
	m_maxChunksRadiusY = 1 + int(m_activitionRange) / LOCAL_Y_SIZE;
	m_maxChunks = (2 * m_maxChunksRadiusX) * (2 * m_maxChunksRadiusY);
	m_deactiveRange = m_activitionRange + LOCAL_X_SIZE + LOCAL_Y_SIZE;
	m_shader = g_theRenderer->CreateShader("Data/Shaders/World");

	CalculateSeed();

	std::string name0 = "oakTree";
	std::string name1 = "cactus";
	std::string name2 = "spruceTree";
	std::string name3 = "house";
	std::string name4 = "pinkHouse";

	std::vector<BlockTemplateEntry> oakTree, cactus, spruceTree, house;
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(0, 0, 0)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(0, 0, 1)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(0, 0, 2)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(1, 0, 2)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(-1, 0, 2)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, 1, 2)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, -1, 2)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(0, 0, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(1, 0, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(-1, 0, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, 1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, -1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(1, -1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(-1, -1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(-1, 1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(1, 1, 3)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, 0, 4)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(1, 0, 4)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(-1, 0, 4)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, 1, 4)));
	oakTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLeaf"), IntVec3(0, -1, 4)));

	cactus.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("cactus"), IntVec3(0, 0, 0)));
	cactus.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("cactus"), IntVec3(0, 0, 1)));
	cactus.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("cactus"), IntVec3(0, 0, 2)));
	cactus.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("cactus"), IntVec3(0, 0, 3)));

	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLog"), IntVec3(0, 0, 0)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLog"), IntVec3(0, 0, 1)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLog"), IntVec3(0, 0, 2)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(1, 0, 2)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(-1, 0, 2)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, 1, 2)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, -1, 2)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLog"), IntVec3(0, 0, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(1, 0, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(-1, 0, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, 1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, -1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(1, -1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(-1, -1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(-1, 1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(1, 1, 3)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, 0, 4)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(1, 0, 4)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(-1, 0, 4)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, 1, 4)));
	spruceTree.push_back(BlockTemplateEntry(BlockDef::GetBlockDefByName("spruceLeaf"), IntVec3(0, -1, 4)));

	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("glowStone"), IntVec3(0, 0, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(1, 0, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, 1, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(-1, 0, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, -1, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(1, -1, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(1, 1, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(-1, -1, 0))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(-1, 1, 0))));

	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, 0, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(1, 0, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, 1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(-1, 0, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, -1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, 1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(-1, -1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(-1, 1, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -2, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -3, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -4, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -5, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -6, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -7, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -8, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -9, 1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -10, 1))));

	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, 0, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(1, 0, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, 1, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(-1, 0, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("air"), IntVec3(0, -1, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, -1, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(1, 1, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(-1, -1, -1))));
	house.push_back((BlockTemplateEntry(BlockDef::GetBlockDefByName("oakLog"), IntVec3(-1, 1, -1))));
	

	BlockTemplate* oakT = new BlockTemplate(name0, oakTree);
	BlockTemplate* cacT = new BlockTemplate(name1, cactus);
	BlockTemplate* sprT = new BlockTemplate(name2, spruceTree);
	BlockTemplate* houseT = new BlockTemplate(name3, "Data/house.3dsprite");
	BlockTemplate* houseT1 = new BlockTemplate(name4, "Data/PinkHouse.3dsprite");
	BlockTemplate* caveLight = new BlockTemplate("caveLight", "Data/CaveLight.3dsprite");
	//BlockTemplate* housT = new BlockTemplate("Data/house.3dsprite");

	BlockTemplate::s_blockTemps.push_back(oakT);
	BlockTemplate::s_blockTemps.push_back(cacT);
	BlockTemplate::s_blockTemps.push_back(sprT);
	BlockTemplate::s_blockTemps.push_back(houseT);
	BlockTemplate::s_blockTemps.push_back(houseT1);
	BlockTemplate::s_blockTemps.push_back(caveLight);
}

World::~World()
{
	for (int chunkIndex = 0; chunkIndex < (int)m_chunks.size(); chunkIndex++) {
		Chunk* chunk = m_chunks[chunkIndex];
		if (chunk != nullptr) {
			delete chunk;
			chunk = nullptr;
		}
	}
	for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
		Chunk* chunk = it->second;
		if (chunk != nullptr) {
			delete chunk;
			chunk = nullptr;
		}
	}
}

void World::Update(float deltaSeconds)
{
	m_nowChunks = (int)m_activeChunks.size();
	m_rebuildMeshTimes = 0;
	if (m_nowChunks < m_maxChunks) {
		IntVec2 nextActive;
		Vec3 playerPos = m_game->m_player->m_position;
		if (FindNearestInactiveChunk(playerPos, nextActive)) {
			Chunk* chunk = new Chunk(nextActive, this);
			if (m_waitForGeneration.find(nextActive) == m_waitForGeneration.end())
			{
				m_waitForGeneration[nextActive] = chunk;
				chunk->m_chunkState = ChunkState::ACTIVATING_QUEUED_GENERATE;
				ChunkGenerateJob* job = new ChunkGenerateJob(chunk);
				g_theJobSystem->AddJobToWaitingQueue(job);
			}
		}
		else {
			FindFarestOutOfDateChunk(playerPos);
		}
	}
	else {
		Vec3 playerPos = m_game->m_player->m_position;
   		FindFarestOutOfDateChunk(playerPos);
	}

	while (1) {
		ChunkGenerateJob* job = (ChunkGenerateJob*)g_theJobSystem->RetrieveJobFromCompletedQueue();
		if (job) {
			Chunk* chunk = job->m_chunk;
			IntVec2 nextActive = chunk->m_chunkCoords;
			m_activeChunks[nextActive] = chunk;
			chunk->m_chunkState = ChunkState::ACTIVE;
			chunk->InitializeLight();
			IntVec2 east, west, north, south;
			east = nextActive;
			west = nextActive;
			north = nextActive;
			south = nextActive;
			east.x += 1;
			west.x -= 1;
			north.y += 1;
			south.y -= 1;
			if (IsChunkActive(east)) {
				chunk->m_eastNeighbor = m_activeChunks[east];
				m_activeChunks[east]->m_westNeighbor = chunk;
			}
			if (IsChunkActive(west)) {
				chunk->m_westNeighbor = m_activeChunks[west];
				m_activeChunks[west]->m_eastNeighbor = chunk;
			}
			if (IsChunkActive(north)) {
				chunk->m_northNeighbor = m_activeChunks[north];
				m_activeChunks[north]->m_southNeighbor = chunk;
			}
			if (IsChunkActive(south)) {
				chunk->m_southNeighbor = m_activeChunks[south];
				m_activeChunks[south]->m_northNeighbor = chunk;
			}
			m_waitlistLock.lock();
			IntVec2 chunkCoords = chunk->m_chunkCoords;
			auto it = m_waitForGeneration.find(chunkCoords);
			if (it != m_waitForGeneration.end()) {
				m_waitForGeneration.erase(it);
			}
			m_waitlistLock.unlock();
			delete job;
		}
		else break;


	}
	m_raycastFace = RaycastWorld(m_game->m_player->m_camera.m_position, m_game->m_player->GetFwdNormal(), 8.f);
	UpdateFromKeyboard(deltaSeconds);
	ProcessDirtyLighting();
	for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
		Chunk* const& chunk = it->second;
		if (chunk != nullptr) {
			chunk->Update(deltaSeconds);
		}
	}

	m_lightningPerlin = Compute1dPerlinNoise(m_game->m_clock->GetTotalSeconds(), 1.f, 9);
	m_glowPerlin = Compute1dPerlinNoise(m_game->m_clock->GetTotalSeconds(), 1.f, 7);
}

void World::Render() const
{
	float dayTime = 0.5f;
		//m_time - (float)RoundDownToInt(m_time);
	Rgba8 skyColor = LerpColor(m_daySkyColor, m_nightSkyColor, RangeMap(GetClamped(fabsf(dayTime - 0.5f), 0.f, 0.25f), 0.f, 0.25f, 0.f, 1.f));
	float lightning = GetClamped(m_lightningPerlin, 0.6f, 0.9f);
	lightning = RangeMap(lightning, 0.6f, 0.9f, 0.f, 1.f);
	skyColor = LerpColor(skyColor, Rgba8::WHITE, lightning);

	float glow = GetClamped(m_glowPerlin, -1.f, 1.f);
	glow = RangeMap(glow, -1.f, 1.f, 0.8f, 1.f);
	g_theRenderer->ClearScreen(skyColor);
	GameConstant gameConstant;
	m_game->m_player->m_position.GetFloat4(gameConstant.m_camWorldPos);
	//gameConstant.m_camWorldPos = camWorldPos;
	m_indoorLightColor.GetAsFloats(gameConstant.m_indoorLightColor);
	for (int i = 0; i < 4; i++) {
		gameConstant.m_indoorLightColor[i] *= glow;
	}
	skyColor.GetAsFloats(gameConstant.m_outdoorLightColor);
	skyColor.GetAsFloats(gameConstant.m_skyColor);
	gameConstant.m_fogStartDist = 20.f;
	gameConstant.m_fogEndDist = 200.f;
	gameConstant.m_fogMaxAlpha = 0.8f;
	gameConstant.m_time = dayTime;

	g_theRenderer->BindShader(m_shader);
	g_theRenderer->UpdateConstantBuffer(g_gameCBO, &gameConstant, sizeof(GameConstant));
	g_theRenderer->BindConstantBuffer(8, g_gameCBO);
	for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
		Chunk* const& chunk = it->second;
		if (chunk != nullptr) {
			chunk->Render();
		}
	}
	if (m_raycastFace.m_didImpact) {
		std::vector<Vertex_PCU> frameVerts;
		Vec3 br, bl, tr, tl;
		m_raycastFace.m_impactBlock.GetFaceWorldBounds(m_raycastFace.m_impactFace, bl, br, tl, tr);
		AddVertsForQuad3D(frameVerts, bl, br, tr, tl);
		g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
		g_theRenderer->SetDepthMode(DepthMode::DISABLED);
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		g_theRenderer->DrawVertexArray((int)frameVerts.size(), frameVerts.data());
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
		g_theRenderer->SetDepthMode(DepthMode::ENABLED);
		g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	}
	g_theRenderer->BindShader(nullptr);
}

int World::GetTotalVertexes() const
{
	int totalNum = 0;
	for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
		Chunk* const& chunk = it->second;
		if (chunk != nullptr) {
			totalNum += (int)chunk->m_cpuMesh.size();
		}
	}
	return totalNum;
}

Vec2 World::GetChunkCenterXYFromCoords(const IntVec2& coords) const
{
	float x = float((coords.x + 0.5f) * LOCAL_X_SIZE);
	float y = float((coords.y + 0.5f) * LOCAL_Y_SIZE);
	return Vec2(x, y);
}

IntVec2 World::GetCoordsFromPosition(const Vec3& position) const
{
	int x = RoundDownToInt(position.x / LOCAL_X_SIZE);
	int y = RoundDownToInt(position.y / LOCAL_Y_SIZE);
	return IntVec2(x, y);
}

bool World::FindNearestInactiveChunk(Vec3 position, IntVec2& coords)
{
	IntVec2 nowCoords = GetCoordsFromPosition(position);
	float minDist = FLT_MAX;
	bool isFound = false;
	for (int xIndex = nowCoords.x - m_maxChunksRadiusX; xIndex < nowCoords.x + m_maxChunksRadiusX; xIndex++) {
		for (int yIndex = nowCoords.y - m_maxChunksRadiusY; yIndex < nowCoords.y + m_maxChunksRadiusX; yIndex++) {
			Vec2 chunkCenter = GetChunkCenterXYFromCoords(IntVec2(xIndex, yIndex));
			if (GetDistanceSquared2D(chunkCenter, position.GetVec2XY()) < m_activitionRange * m_activitionRange) {
				if (!IsChunkActive(IntVec2(xIndex, yIndex))) {
					float dist = GetDistanceSquared2D(chunkCenter, position.GetVec2XY());
					if (dist < minDist) {
						minDist = dist;
						coords.x = xIndex;
						coords.y = yIndex;
						isFound = true;
					}
				}
			}
		}
	}
	return isFound;
}

bool World::FindFarestOutOfDateChunk(Vec3 position)
{
   	float maxDist = -1.f;
	IntVec2 finalCoords;
	bool isFound = false;
	for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
		Chunk* chunk = it->second;
		if (chunk != nullptr) {
			IntVec2 coords = chunk->m_chunkCoords;
			Vec2 chunkPos = GetChunkCenterXYFromCoords(coords);
			//Vec3 distDiff = GetAbsoluteVec3(Vec3(chunkPos) - position);
			float distSquared = GetDistanceSquared2D(position.GetVec2XY(), chunkPos);
			if (distSquared > m_deactiveRange * m_deactiveRange) {
				if (distSquared > maxDist) {
   					maxDist = distSquared;
					finalCoords = coords;
					isFound = true;
				}		
			}
		}
	}
	if (isFound) {
		if (m_waitForGeneration.find(finalCoords) == m_waitForGeneration.end())
		{
			delete m_activeChunks[finalCoords];
			m_activeChunks.erase(finalCoords);
		}
	}
	return isFound;
}



bool World::IsChunkActive(IntVec2 coords)
{

	if (m_activeChunks.count(coords) > 0) {
		return true;
	}
	return false;

}

void World::ProcessDirtyLighting()
{
	while ((int)m_dirtyLightBlocks.size() > 0) {
		BlockIterator blockIterator = m_dirtyLightBlocks.front();
		ProcessNextDirtyLightBlock(blockIterator);
		m_dirtyLightBlocks.pop_front();
	}
	
}

void World::ProcessNextDirtyLightBlock(BlockIterator blockIterator)
{
	Block* block = blockIterator.GetBlock();
	//Block* neighbors[6];
	BlockIterator neighbors[6];
	neighbors[0] = blockIterator.GetEastNeighbor();
	neighbors[1] = blockIterator.GetWestNeighbor();
	neighbors[2] = blockIterator.GetNorthNeighbor();
	neighbors[3] = blockIterator.GetSouthNeighbor();
	neighbors[4] = blockIterator.GetUpNeighbor();
	neighbors[5] = blockIterator.GetDownNeighbor();

	int nowOutdoorLight = block->GetOutdoorLight();
	int nowIndoorLight = block->GetIndoorLight();
	
	bool isChanged = false;
	int indoorDef = BlockDef::s_blockDefs[block->m_blockID]->m_indoorLight;

	int maxIFromN = 0;
	int maxOFromN = 0;

	// calculate self
	if (!block->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
		for (int neighborIndex = 0; neighborIndex < 6; neighborIndex++) {
			//if ()
			BlockIterator neighbor = neighbors[neighborIndex];
			if (!neighbor.IsEmpty()) {
				int indoor = neighbor.GetBlock()->GetIndoorLight();
				int outdoor = neighbor.GetBlock()->GetOutdoorLight();
				if ( indoor > maxIFromN && indoor >= nowIndoorLight) {
					maxIFromN = indoor - 1;
				}
				if ( outdoor > maxOFromN && outdoor >= nowOutdoorLight) {
					maxOFromN = outdoor - 1;
				}
			}
		}
	}

	int maxIndoorLight = GetClamped(indoorDef + maxIFromN, 0, 15);
	int maxOutdoorLight = maxOFromN;
	if (block->GetFlagByType(BlockFlagType::IS_SKY)) {
		maxOutdoorLight += 15;
	}
	maxOutdoorLight = GetClamped(maxOutdoorLight, 0, 15);

	if (nowIndoorLight != maxIndoorLight || nowOutdoorLight != maxOutdoorLight)
		isChanged = true;

	if (isChanged) {
		block->SetIndoorLight((LightInfluence)maxIndoorLight);
		block->SetOutdoorLight((LightInfluence)maxOutdoorLight);
		block->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, false);
		blockIterator.m_chunk->m_isMeshDataDirty = true;

		// dirty others
		for (int neighborIndex = 0; neighborIndex < 6; neighborIndex++) {
			BlockIterator neighbor = neighbors[neighborIndex];
			if (!neighbor.IsEmpty()) {
				neighbor.m_chunk->m_isMeshDataDirty = true;
				Block* nBlock = neighbor.GetBlock();
				if (!nBlock->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
					int indoorN = nBlock->GetIndoorLight();
					int outdoorN = nBlock->GetOutdoorLight();
					if (indoorN + 1 < maxIndoorLight || outdoorN + 1 < maxOutdoorLight || outdoorN < nowOutdoorLight || indoorN < nowIndoorLight) {
						MarkLightingDirty(neighbor);
					}
				}
			}
		}
	}
}

void World::MarkLightingDirty(BlockIterator blockIterator)
{
	if (!blockIterator.IsEmpty()) {
		blockIterator.GetBlock()->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, true);
		m_dirtyLightBlocks.push_back(blockIterator);
	}
}

void World::UndirtyAllBlocksInChunk(Chunk* chunk)
{
	for (auto it = m_dirtyLightBlocks.begin(); it != m_dirtyLightBlocks.end(); it++) {
		BlockIterator iterator = *it;
		if (iterator.m_chunk == chunk) {
			m_dirtyLightBlocks.erase(it);
			it--;
		}
	}
}

void World::CalculateSeed()
{
	m_terrainHeightSeed = m_seed;
	m_hillinessSeed = m_seed + 1;
	m_humiditySeed = m_seed + 2;
	m_temperatureSeed = m_seed + 3;
	m_oceannessSeed = m_seed + 4;
	m_treeSeed = m_seed + 5;
	m_forestnessSeed = m_seed + 6;
	m_caveSeed = m_seed + 7;
	m_caveSeed = m_seed + 8;
}

void World::CalculateCorrectLightValue(BlockIterator blockIterator)
{
	Block* block = blockIterator.GetBlock();
	//Block* neighbors[6];
	BlockIterator neighbors[6];
	neighbors[0] = blockIterator.GetEastNeighbor();
	neighbors[1] = blockIterator.GetWestNeighbor();
	neighbors[2] = blockIterator.GetNorthNeighbor();
	neighbors[3] = blockIterator.GetSouthNeighbor();
	neighbors[4] = blockIterator.GetUpNeighbor();
	neighbors[5] = blockIterator.GetDownNeighbor();

	LightInfluence nowOutdoorLight = block->GetOutdoorLight();
	LightInfluence nowIndoorLight = block->GetIndoorLight();
	LightInfluence maxOutdoorLight = nowOutdoorLight;
	LightInfluence maxIndoorLight = nowIndoorLight;

	// calculate self
	for (int neighborIndex = 0; neighborIndex < 6; neighborIndex++) {
		//if ()
		BlockIterator neighbor = neighbors[neighborIndex];
		if (!neighbor.IsEmpty()) {
			LightInfluence indoor = neighbor.GetBlock()->GetIndoorLight();
			LightInfluence outdoor = neighbor.GetBlock()->GetOutdoorLight();
			if (indoor - 1 > maxIndoorLight) {
				maxIndoorLight = indoor - 1;
			}
			if (outdoor - 1 > maxOutdoorLight) {
				maxOutdoorLight = outdoor - 1;
			}
		}
	}
	block->SetIndoorLight(maxIndoorLight);
	block->SetOutdoorLight(maxOutdoorLight);
	block->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, false);

	// dirty others
	for (int neighborIndex = 0; neighborIndex < 6; neighborIndex++) {
		BlockIterator neighbor = neighbors[neighborIndex];
		if (!neighbor.IsEmpty()) {
			Block* nBlock = neighbor.GetBlock();
			if (!nBlock->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
				LightInfluence indoor = nBlock->GetIndoorLight();
				LightInfluence outdoor = nBlock->GetOutdoorLight();
				if (indoor + 1 < maxIndoorLight) {
					MarkLightingDirty(neighbor);
				}
				if (outdoor + 1 < maxOutdoorLight) {
					MarkLightingDirty(neighbor);
				}
			}
		}
	}
}

GameRaycastResult World::RaycastWorld(const Vec3& start, const Vec3& direction, float distance)
{
	GameRaycastResult result;
	result.m_rayStartPos = start;
	result.m_rayFwdNormal = direction;
	result.m_rayMaxLength = distance;
	IntVec2 chunkCoords = GetCoordsFromPosition(start);
	if (!IsChunkActive(chunkCoords)) {
		return result;
	}
	Chunk* chunk = m_activeChunks[chunkCoords];
	if (chunk == nullptr)
		return result;
	IntVec3 startCoords = chunk->GetWorldBlockCoordsByPosition(start);
	IntVec3 sLocalCoords = chunk->GetLocalBlockCoordsByPosition(start);
	int index = chunk->GetBlockIndexByCoords(sLocalCoords);
	BlockIterator blockIt = BlockIterator(chunk, index);
	int tileX = startCoords.x;
	int tileY = startCoords.y;
	int tileZ = startCoords.z;
	if (blockIt.IsEmpty())
		return result;
	bool isStartSolid = blockIt.GetBlock()->GetFlagByType(BlockFlagType::IS_SOLID);
	if (isStartSolid) {
		result.m_didImpact = true;
		result.m_impactPos = start;
		result.m_impactNormal = -direction;
		return result;
	}
	float fwdDistPerXCrossing = 0.f;
	int tileStepDirX = 0;
	float xAtFirstXCrossing = (float)tileX;
	float xDistToFirstXCrossing = 0.f;
	float fwdDistAtNextXCrossing = 99999.f;
	if (direction.x != 0.f) {
		fwdDistPerXCrossing = 1.f / abs(direction.x);
		if (direction.x < 0.f)
			tileStepDirX = -1;
		else tileStepDirX = 1;
		xAtFirstXCrossing += (tileStepDirX + 1) / 2;
		xDistToFirstXCrossing = xAtFirstXCrossing - start.x;
		fwdDistAtNextXCrossing = fabsf(xDistToFirstXCrossing) * fwdDistPerXCrossing;
	}

	float fwdDistPerYCrossing = 0.f;
	int tileStepDirY = 0;
	float yAtFirstYCrossing = (float)tileY;
	float yDistToFirstYCrossing = 0.f;
	float fwdDistAtNextYCrossing = 99999.f;
	if (direction.y != 0.f) {
		fwdDistPerYCrossing = 1.f / abs(direction.y);
		if (direction.y < 0.f)
			tileStepDirY = -1;
		else tileStepDirY = 1;
		yAtFirstYCrossing += (tileStepDirY + 1) / 2;
		yDistToFirstYCrossing = yAtFirstYCrossing - start.y;
		fwdDistAtNextYCrossing = fabsf(yDistToFirstYCrossing) * fwdDistPerYCrossing;
	}

	float fwdDistPerZCrossing = 0.f;
	int tileStepDirZ = 0;
	float zAtFirstZCrossing = (float)tileZ;
	float zDistToFirstZCrossing = 0.f;
	float fwdDistAtNextZCrossing = 99999.f;
	if (direction.z != 0.f) {
		fwdDistPerZCrossing = 1.f / abs(direction.z);
		if (direction.z < 0.f)
			tileStepDirZ = -1;
		else tileStepDirZ = 1;
		zAtFirstZCrossing += (tileStepDirZ + 1) / 2;
		zDistToFirstZCrossing = zAtFirstZCrossing - start.z;
		fwdDistAtNextZCrossing = fabsf(zDistToFirstZCrossing) * fwdDistPerZCrossing;
	}
	while (1) {
		if (MinNumAmong3(fwdDistAtNextXCrossing, fwdDistAtNextYCrossing, fwdDistAtNextZCrossing) == fwdDistAtNextXCrossing) {
			if (fwdDistAtNextXCrossing > distance) {
				return result;
			}
			if (tileStepDirX > 0) {
				blockIt = blockIt.GetEastNeighbor();
			}
			else
			{
				blockIt = blockIt.GetWestNeighbor();
			}
			//tileX += tileStepDirX;
			if (blockIt.IsEmpty()) {
				return result;
			}
			Block* block = blockIt.GetBlock();

			if (block == nullptr)
				return result;
			if (block->GetFlagByType(BlockFlagType::IS_SOLID)) {
				result.m_didImpact = true;
				result.m_impactDis = fwdDistAtNextXCrossing;
				result.m_impactPos = start + direction * fwdDistAtNextXCrossing;
				result.m_impactBlock = blockIt;
				if (tileStepDirX > 0) {
					result.m_impactFace = FaceDirt::WEST;
					result.m_impactNormal = Vec3(-1.f, 0.f, 0.f);
				}

				else {
					result.m_impactFace = FaceDirt::EAST;
					result.m_impactNormal = Vec3(1.f, 0.f, 0.f);
				}
				return result;
			}
			fwdDistAtNextXCrossing += fwdDistPerXCrossing;
		}
		else if (MinNumAmong3(fwdDistAtNextXCrossing, fwdDistAtNextYCrossing, fwdDistAtNextZCrossing) == fwdDistAtNextYCrossing) {
			if (fwdDistAtNextYCrossing > distance) {
				return result;
			}
			if (tileStepDirY > 0) {
				blockIt = blockIt.GetNorthNeighbor();
			}
			else
			{
				blockIt = blockIt.GetSouthNeighbor();
			}
			//tileX += tileStepDirX;
			if (blockIt.IsEmpty()) {
				return result;
			}
			Block* block = blockIt.GetBlock();

			if (block == nullptr)
				return result;

			if (block->GetFlagByType(BlockFlagType::IS_SOLID)) {
				result.m_didImpact = true;
				result.m_impactDis = fwdDistAtNextYCrossing;
				result.m_impactPos = start + direction * fwdDistAtNextYCrossing;
				result.m_impactBlock = blockIt;
				if (tileStepDirY > 0) {
					result.m_impactFace = FaceDirt::SOUTH;
					result.m_impactNormal = Vec3(0.f, -1.f, 0.f);
				}

				else {
					result.m_impactFace = FaceDirt::NORTH;
					result.m_impactNormal = Vec3(0.f, 1.f, 0.f);
				}
				return result;
			}
			fwdDistAtNextYCrossing += fwdDistPerYCrossing;
			
		}
		else if (MinNumAmong3(fwdDistAtNextXCrossing, fwdDistAtNextYCrossing, fwdDistAtNextZCrossing) == fwdDistAtNextZCrossing){
			if (fwdDistAtNextZCrossing > distance) {
				return result;
			}
			if (tileStepDirZ > 0) {
				blockIt = blockIt.GetUpNeighbor();
			}
			else
			{
				blockIt = blockIt.GetDownNeighbor();
			}
			//tileX += tileStepDirX;
			if (blockIt.IsEmpty()) {
				return result;
			}
			Block* block = blockIt.GetBlock();
			if (block == nullptr)
				return result;
			if (block->GetFlagByType(BlockFlagType::IS_SOLID)) {
				result.m_didImpact = true;
				result.m_impactDis = fwdDistAtNextZCrossing;
				result.m_impactPos = start + direction * fwdDistAtNextZCrossing;
				result.m_impactBlock = blockIt;
				if (tileStepDirZ > 0) {
					result.m_impactFace = FaceDirt::DOWN;
					result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
				}

				else {
					result.m_impactFace = FaceDirt::UP;
					result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
				}
				return result;
			}
			fwdDistAtNextZCrossing += fwdDistPerZCrossing;
		}
	}
	return GameRaycastResult();
}

void World::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		if (m_raycastFace.m_didImpact) {
			m_raycastFace.m_impactBlock.GetBlock()->m_blockID = BlockDef::GetBlockDefByName("air");
			m_raycastFace.m_impactBlock.GetBlock()->SetFlagsByDef();
			m_raycastFace.m_impactBlock.m_chunk->m_isMeshDataDirty = true;
			m_raycastFace.m_impactBlock.m_chunk->m_isSaveDirty = true;
			MarkLightingDirty(m_raycastFace.m_impactBlock);
			BlockIterator blockIt = m_raycastFace.m_impactBlock;
			BlockIterator upNeighbor = blockIt.GetUpNeighbor();
			if (!upNeighbor.IsEmpty()) {
				if (upNeighbor.GetBlock()->GetFlagByType(BlockFlagType::IS_SKY)) {
					blockIt.GetBlock()->SetFlagByType(BlockFlagType::IS_SKY, true);
					while (1) {
						blockIt = blockIt.GetDownNeighbor();
						if (blockIt.IsEmpty())
							break;
						if (blockIt.GetBlock()->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE))
							break;
						blockIt.GetBlock()->SetFlagByType(BlockFlagType::IS_SKY, true);
						MarkLightingDirty(blockIt);
					}
				}
			}

		}
	}

	if (g_theInput->IsKeyDown('Y')) {
		m_time += deltaSeconds * m_timeScaler * 0.01f;
	}
	else
	{
		m_time += deltaSeconds * 0.01f;
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE)) {
		if (m_raycastFace.m_didImpact) {
			BlockIterator blockIt = m_raycastFace.m_impactBlock.GetNeighborByFace(m_raycastFace.m_impactFace);

			if (m_isPlacingCobbleStone) {
				blockIt.GetBlock()->m_blockID = BlockDef::GetBlockDefByName("cobbleStone");
				
				//chunk->m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("cobbleStone");
				blockIt.GetBlock()->SetFlagsByDef();
				blockIt.GetBlock()->SetFlagByType(BlockFlagType::IS_SKY, false);
				MarkLightingDirty(blockIt);
				while (1) {
					blockIt = blockIt.GetDownNeighbor();
					if (blockIt.IsEmpty())
						break;
					if (blockIt.GetBlock()->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE) || 
						!blockIt.GetBlock()->GetFlagByType(BlockFlagType::IS_SKY))
						break;
					blockIt.GetBlock()->SetFlagByType(BlockFlagType::IS_SKY, false);
					MarkLightingDirty(blockIt);
				}
				
			}
			else
			{
				blockIt.GetBlock()->m_blockID = BlockDef::GetBlockDefByName("glowStone");
				blockIt.GetBlock()->SetFlagsByDef();
				MarkLightingDirty(blockIt);
			}
			blockIt.m_chunk->m_isMeshDataDirty = true;
			blockIt.m_chunk->m_isSaveDirty = true;
		}
		IntVec2 chunkCoords = GetCoordsFromPosition(m_game->m_player->m_position);
		Chunk* chunk = m_activeChunks[chunkCoords];
		IntVec3 blockCoords = chunk->GetFirstOneNotAir(m_game->m_player->m_position);
		blockCoords.z += 1;
	}
	if (g_theInput->WasKeyJustPressed('1')) {
		m_isPlacingCobbleStone = !m_isPlacingCobbleStone;
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F8)) {
		for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
			Chunk* chunk = it->second;
			if (chunk != nullptr) {
				delete chunk;
			}
		}
		m_activeChunks.clear();
		m_dirtyLightBlocks.clear();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F7)) {
		for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
			Chunk* chunk = it->second;
			if (chunk != nullptr) {
				delete chunk;
			}
		}
		m_activeChunks.clear();
		m_dirtyLightBlocks.clear();
		m_seed++;
		CalculateSeed();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_F9)) {
		for (auto it = m_activeChunks.begin(); it != m_activeChunks.end(); it++) {
			Chunk* chunk = it->second;
			if (chunk != nullptr) {
				delete chunk;
			}
		}
		m_activeChunks.clear();
		m_dirtyLightBlocks.clear();
		m_seed--;
		CalculateSeed();
	}

	if (g_theInput->WasKeyJustPressed('R')) {
		if (m_raycastFace.m_didImpact) {
			DebugAddWorldLine(m_raycastFace.m_rayStartPos, m_raycastFace.m_impactPos, 0.01f, 10.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::X_RAY);
			DebugAddWorldLine(m_raycastFace.m_impactPos, m_raycastFace.m_rayStartPos + m_raycastFace.m_rayFwdNormal * m_raycastFace.m_rayMaxLength, 0.01f, 10.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY);
			DebugAddWorldLine(m_raycastFace.m_impactPos + Vec3(0.05f, 0.05f, 0.05f), m_raycastFace.m_impactPos - Vec3(0.05f, 0.05f, 0.05f), 0.01f, 10.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::X_RAY);
			DebugAddWorldLine(m_raycastFace.m_impactPos + Vec3(0.05f, 0.05f, -0.05f), m_raycastFace.m_impactPos - Vec3(0.05f, 0.05f, -0.05f), 0.01f, 10.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::X_RAY);
			DebugAddWorldLine(m_raycastFace.m_impactPos + Vec3(-0.05f, 0.05f, 0.05f), m_raycastFace.m_impactPos - Vec3(-0.05f, 0.05f, 0.05f), 0.01f, 10.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::X_RAY);
		}
		
	}
}

