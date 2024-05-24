#define UNUSED(x) (void)(x);
#include "Game/Prop.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Game/BlockDef.hpp"
#include "Game/BlockTemplate.hpp"
#include "Game/BlockIterator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"

Chunk::Chunk(IntVec2 coords, World* world)
{
	m_world = world;
	m_chunkCoords = coords;
	m_worldBounds.m_mins.x = (float)(coords.x * LOCAL_X_SIZE);
	m_worldBounds.m_mins.y = (float)(coords.y * LOCAL_Y_SIZE);
	m_worldBounds.m_mins.z = 0.f;

	m_worldBounds.m_maxs.x = (float)((coords.x + 1) * LOCAL_X_SIZE);
	m_worldBounds.m_maxs.y = (float)((coords.y + 1) * LOCAL_Y_SIZE);
	m_worldBounds.m_maxs.z = 0.f + float(LOCAL_Z_SIZE);

	m_blocks = new Block[BLOCK_TOTAL_NUMBER];
	PopulateBlocks();
}

Chunk::~Chunk()
{
	if (m_isSaveDirty) {
		SaveToFile();
	}
	delete m_gpuMesh;
	m_gpuMesh = nullptr;
	m_cpuMesh.clear();
	delete[] m_blocks;
	m_blocks = nullptr;

	if (m_eastNeighbor)
		m_eastNeighbor->m_westNeighbor = nullptr;
	if (m_westNeighbor)
		m_westNeighbor->m_eastNeighbor = nullptr;
	if (m_northNeighbor)
		m_northNeighbor->m_southNeighbor = nullptr;
	if (m_southNeighbor)
		m_southNeighbor->m_northNeighbor = nullptr;
}

void Chunk::PopulateBlocks()
{
	bool isLoadable = LoadFromFile();

	if (isLoadable)
		return;
	
	for (int xIndex = 0; xIndex < LOCAL_X_SIZE; xIndex++) {
		int yIndex = 0;
		int globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		int globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		yIndex = 1;
		//globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		yIndex = LOCAL_Y_SIZE - 1;
		//globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		yIndex = LOCAL_Y_SIZE - 2;
		//globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
	}

	for (int yIndex = 0; yIndex < LOCAL_Y_SIZE; yIndex++) {
		int xIndex = 0;
		int globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		int globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		xIndex = 1;
		globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		//globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		xIndex = LOCAL_X_SIZE - 1;
		globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		//globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
		xIndex = LOCAL_X_SIZE - 2;
		globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
		//globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;
		IsLeafColumn(globalX, globalY, m_world->m_treeSeed);
	}
	
	AddCaves();
	AddHouse();

	for (int xIndex = 0; xIndex < LOCAL_X_SIZE; xIndex++) {
		for (int yIndex = 0; yIndex < LOCAL_Y_SIZE; yIndex++) {
			int globalX = xIndex + m_chunkCoords.x * LOCAL_X_SIZE;
			int globalY = yIndex + m_chunkCoords.y * LOCAL_Y_SIZE;

			float treeNoise = 0.f;

			if (IsTreeColumn(globalX, globalY, m_world->m_treeSeed)) {
				treeNoise = Get2dNoiseZeroToOne(globalX, globalY, m_world->m_treeSeed);
			}
			
			

			float hilliness =  SmoothStep3(0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 3, 0.5f, 2.f, m_world->m_hillinessSeed));
			//hilliness = SmoothStep3(hilliness);
			int hillheight = RoundDownToInt( 74 * hilliness);
			int terrainHeightZ = 62 + int( fabsf(Compute2dPerlinNoise(float(globalX), float(globalY), 200.f, 5, 0.5f, 2.f, m_world->m_terrainHeightSeed)) * hillheight);
			float humidity	  = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 200.f, 3, 0.5f, 2.f, m_world->m_humiditySeed);
			float temperature = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 5, 0.5f, 2.f, m_world->m_temperatureSeed);
			float oceanness = GetClamped(Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 3, 0.5f, 2.f, m_world->m_oceannessSeed), 0.f, 0.5f);
			oceanness = RangeMap(oceanness, 0.f, 0.5f, 0.f, 1.f);
			oceanness = SmoothStep3(oceanness);
			int maxOceanDepth = 40;
			int nowOceanDepth = RoundDownToInt(maxOceanDepth * oceanness);
			terrainHeightZ -= nowOceanDepth;
			//SEA_LEVEL + (globalX / 3) + (globalY / 5) + g_rng->RollRandomIntInRange(0, 1);
			int dirtHeightZ = terrainHeightZ - g_rng->RollRandomIntInRange(3, 4);
			bool moreSky = true;

			float forestness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 800.f, 3, 0.5f, 2.f, m_world->m_forestnessSeed);

			float treeThreshold = RangeMapClamped(forestness, 0.5f, 1.f, 1.f, 0.9f);

			bool isTree = false;

			if (treeNoise > treeThreshold && terrainHeightZ > LOCAL_Z_SIZE / 2)
				isTree = true;
			for (int zIndex = LOCAL_Z_SIZE - 1; zIndex >= 0; zIndex--) {
				int index = GetBlockIndexByCoords(IntVec3(xIndex, yIndex, zIndex));

				// if has been set, then do nothing and set more sky false if neccessary
				if (m_blocks[index].m_blockID != 255) {
					if (m_blocks[index].GetFlagByType(BlockFlagType::IS_SOLID)) {
						moreSky = false;
					}
					if (moreSky) {
						m_blocks[index].SetAsSky();
					}
					/*else if (m_blocks[index].m_blockID == BlockDef::GetBlockDefByName("air") && moreSky)
					{
						m_blocks[index].SetAsSky();
					}*/
					continue;
				}

				if (zIndex == terrainHeightZ) {
					if (terrainHeightZ == LOCAL_Z_SIZE / 2 && humidity < 0.6f) {
						m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("sand");
					}
					else if (humidity > 0.2f)
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Grass;
					else
						m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("sand");
					//BlockDef::GetBlockDefByName("grass");
				}
				else if (zIndex < dirtHeightZ)
				{
					int chance = g_rng->RollRandomIntInRange(0, 999);
					if (chance == 0) {
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Diamond;
						//BlockDef::GetBlockDefByName("diamond");
					}
					else if (chance >= 1 && chance <= 5) {
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Gold;
						//BlockDef::GetBlockDefByName("gold");
					}
					else if (chance >= 6 && chance <= 25) {
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Iron;
						//BlockDef::GetBlockDefByName("iron");
					}
					else if (chance >= 26 && chance <= 75) {
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Coal;
						//BlockDef::GetBlockDefByName("coal");
					}
					else {
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Stone;
						//BlockDef::GetBlockDefByName("stone");
					}
				}
				else if (zIndex >= dirtHeightZ && zIndex < terrainHeightZ)
				{
					if (humidity > 0.2f)
						m_blocks[index].m_blockID = (BlockIDType)BlockType::Dirt;
					else
					{
						if (zIndex > terrainHeightZ - (1 - temperature) * 5) {
							m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("sand");
						}
						else
						{
							m_blocks[index].m_blockID = (BlockIDType)BlockType::Dirt;
						}
					}
					//BlockDef::GetBlockDefByName("dirt");
				}
				else {
					if (zIndex <= LOCAL_Z_SIZE / 2) {
						if (temperature > 0.4f)
							m_blocks[index].m_blockID = (BlockIDType)BlockType::Water;
						else {
							if (zIndex > LOCAL_Z_SIZE / 2 - (1 - temperature) * 5)
								m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("ice");
							else 
								m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("water");
						}
						//BlockDef::GetBlockDefByName("water");
					}
					else
					{
						if (isTree && zIndex == terrainHeightZ + 1) {
							//m_blocks[index].m_blockID = BlockDef::GetBlockDefByName("oakLog");
							if (humidity < 0.2f)
								SpawnBlockTemplateAtCoords("cactus", IntVec3(xIndex, yIndex, zIndex));
							else if (temperature < 0.4f) {
								SpawnBlockTemplateAtCoords("spruceTree", IntVec3(xIndex, yIndex, zIndex));
							}
							else {
								SpawnBlockTemplateAtCoords("oakTree", IntVec3(xIndex, yIndex, zIndex));
							}
						}
						else m_blocks[index].m_blockID = (BlockIDType)BlockType::Air;
						//BlockDef::GetBlockDefByName("air");
					}
				}

				m_blocks[index].SetFlagsByDef();

				if (m_blocks[index].GetFlagByType(BlockFlagType::IS_SOLID)) {
					moreSky = false;
				}
				if (moreSky) {
					m_blocks[index].SetAsSky();
				}

			}
		}
	}

}
void Chunk::InitializeLight()
{
	for (int index = 0; index < BLOCK_TOTAL_NUMBER; index++) {
		Block* block = &m_blocks[index];
		BlockIterator blockIt = BlockIterator(this, index);
		if (block->GetFlagByType(BlockFlagType::IS_SKY)) {
			std::vector<BlockIterator> horizontalNeighbors;
			horizontalNeighbors.reserve(4);
			//BlockIterator blockIt = BlockIterator(this, index);
			blockIt.AddHorizontalNeighbor(horizontalNeighbors);
			for (int neighborIndex = 0; neighborIndex < (int)horizontalNeighbors.size(); neighborIndex++) {
				BlockIterator temp = horizontalNeighbors[neighborIndex];
				Block* blockT = temp.GetBlock();
				if (!blockT->GetFlagByType(BlockFlagType::IS_SKY) && !blockT->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {

					blockT->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, true);
				}
			}
		}
		if (IsBlockOnBorder(index) && !block->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
			block->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, true);
		}
		if (block->GetFlagByType(BlockFlagType::IS_LIGHT_DIRTY)) {
			m_world->MarkLightingDirty(blockIt);
		}

	}
}

bool Chunk::LoadFromFile()
{
	std::vector<uint8_t> fileContent;
	std::string filename = Stringf("Saves/World_%u/Chunk(%d,%d).chunk", m_world->m_seed, m_chunkCoords.x, m_chunkCoords.y);
	if (!IsFileExist(filename)) {
		return false;
	}
	FileReadToBuffer(fileContent, filename);
	
	if (fileContent.size() <= 8) {
		return false;
	}

	if (fileContent[0] != 'G' ||
		fileContent[1] != 'C' ||
		fileContent[2] != 'H' ||
		fileContent[3] != 'K' ||
		fileContent[4] != m_world->m_version ||
		fileContent[5] != X_BIT_SIZE ||
		fileContent[6] != Y_BIT_SIZE ||
		fileContent[7] != Z_BIT_SIZE) {
		//ERROR_AND_DIE(Stringf("Bad file headers %s", filename.c_str()));
		return false;
	}
	unsigned int seed = fileContent[8] << 24 | fileContent[9] << 16 | fileContent[10] << 8 | fileContent[11];
	if (seed != m_world->m_seed)
		return false;
	int totalCount = 0;
	for (int charIndex = 8; charIndex < (int)fileContent.size(); charIndex+=2) {
		BlockIDType defIndex = (BlockIDType)fileContent[charIndex];
		int countIndex = charIndex + 1;
		int count = fileContent[countIndex];
		//BlockDef* blockdef = BlockDef::s_blockDefs[defIndex];
		for (int blockIndex = 0; blockIndex < count; blockIndex++) {
			m_blocks[totalCount].m_blockID = defIndex;
			m_blocks[totalCount].SetFlagsByDef();
			totalCount++;
		}
	}
	if (totalCount == BLOCK_TOTAL_NUMBER) {
		for (int xIndex = 0; xIndex < LOCAL_X_SIZE; xIndex++) {
			for (int yIndex = 0; yIndex < LOCAL_Y_SIZE; yIndex++) {
				bool moreSky = true;
				for (int zIndex = LOCAL_Z_SIZE - 1; zIndex >= 0; zIndex--) {
					int index = GetBlockIndexByCoords(IntVec3(xIndex, yIndex, zIndex));

					if (m_blocks[index].GetFlagByType(BlockFlagType::IS_SOLID)) {
						moreSky = false;
					}
					if (moreSky) {
						m_blocks[index].SetAsSky();
						m_blocks[index].SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, false);
						std::vector<BlockIterator> horizontalNeighbors;
						BlockIterator blockIt = BlockIterator(this, index);
						blockIt.AddHorizontalNeighbor(horizontalNeighbors);
						for (int neighborIndex = 0; neighborIndex < (int)horizontalNeighbors.size(); neighborIndex++) {
							BlockIterator temp = horizontalNeighbors[neighborIndex];
							Block* block = temp.GetBlock();
							if (!block->GetFlagByType(BlockFlagType::IS_SKY) && !block->GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
								block->SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, true);					
								//m_world->MarkLightingDirty(BlockIterator(this, ));
							}
						}
					}

				}
			}
		}
		for (int index = 0; index < BLOCK_TOTAL_NUMBER; index++) {
			Block* block = &m_blocks[index];
			BlockIterator blockIt = BlockIterator(this, index);
			if (block->GetFlagByType(BlockFlagType::IS_LIGHT_DIRTY)) {
				m_world->MarkLightingDirty(blockIt);
			}

		}
		return true;
	}
	return false;
}

bool Chunk::SaveToFile()
{
	std::vector<uint8_t> fileContent;
	std::string filename = Stringf("Saves/World_%u/Chunk(%d,%d).chunk", m_world->m_seed, m_chunkCoords.x, m_chunkCoords.y);
	fileContent.resize(12);
	fileContent[0] = 'G';
	fileContent[1] = 'C';
	fileContent[2] = 'H';
	fileContent[3] = 'K';
	fileContent[4] = m_world->m_version;
	fileContent[5] = X_BIT_SIZE;
	fileContent[6] = Y_BIT_SIZE;
	fileContent[7] = Z_BIT_SIZE;
	// 0000 0000 / 0000 0000 / 0000 0000 / 0000 0000 save the seed in 4 unsigned char
	fileContent[8] = m_world->m_seed >> 24;
	// mask 0000 0000 / 1111 1111 / 0000 0000 / 0000 0000
	unsigned int mask = 255 << 16;
	fileContent[9] = unsigned char((mask & m_world->m_seed) >> 16);
	mask = 255 << 8;
	fileContent[10] = unsigned char((mask & m_world->m_seed) >> 8);
	mask = 255;
	fileContent[11] = unsigned char(mask & m_world->m_seed);
	int defIndex = -1;
	uint8_t count = 0;
	for (int blockIndex = 0; blockIndex < BLOCK_TOTAL_NUMBER; blockIndex++) {
		if (m_blocks[blockIndex].m_blockID != defIndex || count >= 255) {
			if (defIndex >= 0)
				fileContent.push_back((uint8_t)defIndex);
			if (count > 0)
				fileContent.push_back(count);
			count = 1;
			defIndex = m_blocks[blockIndex].m_blockID;
		}
		else
		{
			count++;
		}
	}
	if (count > 1) {
		fileContent.push_back(m_blocks[BLOCK_TOTAL_NUMBER - 1].m_blockID);
		fileContent.push_back(count);
	}
	WriteToFile(fileContent, filename);
	m_isSaveDirty = false;
	return true;
}

void Chunk::RebuildChunkMeshVerts()
{
	m_cpuMesh.clear();
	constexpr int SIDE_PER_BLOCK = 6;
	constexpr int TRI_PER_SIDE = 2;
	constexpr int VERT_PER_TRI = 3;
	m_cpuMesh.reserve(BLOCK_TOTAL_NUMBER);
	for (int blockIndex = 0; blockIndex < BLOCK_TOTAL_NUMBER; blockIndex++) {
		AddVertsForBlock(m_cpuMesh, blockIndex);
	}
	if (m_gpuMesh != nullptr)
	{
		delete m_gpuMesh;
		m_gpuMesh = nullptr;
	}
	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * BLOCK_TOTAL_NUMBER, sizeof(Vertex_PCU));
	g_theRenderer->CopyCPUToGPU(m_cpuMesh.data(), m_cpuMesh.size() * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_gpuMesh);
	m_isMeshDataDirty = false;
	m_world->m_rebuildMeshTimes++;
}
void Chunk::SpawnBlockTemplateAtCoords(std::string tempName, IntVec3 coords)
{
	BlockTemplate* temp = BlockTemplate::GetTemplateByName(tempName);
	if (temp == nullptr) {
		ERROR_AND_DIE(Stringf("Can't find a block template named %s", tempName.c_str()));
		return;
	}
	for (int blockIndex = 0; blockIndex < (int)temp->m_blocks.size(); blockIndex++) {
		BlockTemplateEntry& entry = temp->m_blocks[blockIndex];
		IntVec3 nowCoords = coords + entry.m_coords;
		if (IsCoordsInBounds(nowCoords)) {
			int index = GetBlockIndexByCoords(nowCoords);
			m_blocks[index].m_blockID = entry.m_blockDef;
			m_blocks[index].SetFlagsByDef();
		}
	}
}

void Chunk::AddCaves()
{
	// Calculate how big of a search area we'll need to check chunks in to detect for caves
	constexpr float CHUNK_WIDTH = static_cast<float>(LOCAL_X_SIZE < LOCAL_Y_SIZE ? LOCAL_X_SIZE : LOCAL_Y_SIZE);
	constexpr float CAVE_MAX_DISTANCE_BLOCKS = 100.f;
	constexpr int CAVE_MAX_STEP = 20;
	constexpr int CAVE_SEARCH_RADIUS = 1 + int(CAVE_MAX_DISTANCE_BLOCKS / CHUNK_WIDTH);
	constexpr float CAVE_THRESHOLD = 0.99f;
	IntVec2 chunkSearchMins = m_chunkCoords - IntVec2(CAVE_SEARCH_RADIUS, CAVE_SEARCH_RADIUS);
	IntVec2 chunkSearchMaxs = m_chunkCoords + IntVec2(CAVE_SEARCH_RADIUS, CAVE_SEARCH_RADIUS);

	std::vector<IntVec3> startCoords;
	// Get a cave noise value based on chunk coords for each chunk in a huge region around me
	// Each chunk with a noise value > some threshold has a cave that STARTS in that chunk
	//Each cave will also need a ,pst;u-unique seed number for its RNG to make it different
	for (int chunkY = chunkSearchMins.y; chunkY <= chunkSearchMaxs.y; chunkY++) {
		for (int chunkX = chunkSearchMins.x; chunkX <= chunkSearchMaxs.x; chunkX++) {
			float caveOriginationNoise = Get2dNoiseZeroToOne(chunkX, chunkY, m_world->m_caveSeed);
			if (caveOriginationNoise < CAVE_THRESHOLD) {
				continue;
			}
			// calculate the startPosition
			int z = GetTerrainHeightZ(chunkX, chunkY);
			z -= 2;
			RandomNumberGenerator rng;
			rng.m_seed = chunkX + 1234567 * chunkY;
			int x = rng.RollRandomIntInRange(0, 15) + chunkX * LOCAL_X_SIZE;
			int y = rng.RollRandomIntInRange(0, 15) + chunkY * LOCAL_Y_SIZE;
			startCoords.emplace_back(x, y, z);
		}
	}
	for (int caveIndex = 0; caveIndex < (int)startCoords.size(); caveIndex++) {

		IntVec3 startPos = startCoords[caveIndex];
		RandomNumberGenerator rng;
		rng.m_seed = startPos.x + startPos.y * 1324576;
		IntVec3 lastPos = startPos;
		std::vector<IntVec3> caveNodes;
		//caveNodes.push_back(lastPos);
		for (int stepIndex = 0; stepIndex < CAVE_MAX_STEP; stepIndex++) {
			//float yaw = rng.RollRandomFloatInRange(-60.f, 60.f);
			EulerAngles orientation;
			orientation.m_yawDegrees = rng.RollRandomFloatInRange(-120.f, 120.f);
			orientation.m_pitchDegrees = rng.RollRandomFloatInRange(-10.f, 60.f);

			float radius = rng.RollRandomFloatInRange(2.f, 6.f);

			Vec3 nowForward = orientation.GetForwardVector() * 10.f;
			IntVec3 nowPos;
			nowPos.x = RoundDownToInt(nowForward.x);
			nowPos.y = RoundDownToInt(nowForward.y);
			nowPos.z = RoundDownToInt(nowForward.z);
			nowPos = nowPos + lastPos;
			// if not within the range
			if ((startPos - nowPos).GetLengthSquared() > 10000)
				break;
			//DebugAddWorldPoint()
			//AddVertsForSphere3D(m_cpuMesh, )
			Vec3 nowPosVec3, lastPosVec3;
			nowPosVec3.x = (float)nowPos.x + 0.5f;
			nowPosVec3.y = (float)nowPos.y + 0.5f;
			nowPosVec3.z = (float)nowPos.z + 0.5f;

			lastPosVec3.x = (float)lastPos.x + 0.5f;
			lastPosVec3.y = (float)lastPos.y + 0.5f;
			lastPosVec3.z = (float)lastPos.z + 0.5f;

			Vec3 center = (nowPosVec3 + lastPosVec3) * 0.5f;
			float halfDist = (nowPosVec3 - center).GetLength();
			Vec3 nearest = m_worldBounds.GetNearestPoint(center);

			// if not within the range of the sphere
			if ((nearest - nowPosVec3).GetLengthSquared() > (radius + halfDist) * (radius + halfDist) ) {
				lastPos = nowPos;
				continue;
			}

			
			float minDist = radius * radius;
			int index = -1;
			for (int blockIndex = 0; blockIndex < BLOCK_TOTAL_NUMBER; blockIndex++) {
				Vec3 blockCenter = GetBlockWorldCenterByIndex(blockIndex);
				//Vec3 centerToEnd = nowPosVec3 - blockCenter;
				//Vec3 startToEnd = nowPosVec3 - lastPosVec3;
				//Vec3 projection = GetProjectedOnto3D(centerToEnd, startToEnd);
				//Vec3 distance = centerToEnd - projection;
				Vec3 nearestToLine = GetNearestPointOnLineSegment3D(blockCenter, lastPosVec3, nowPosVec3);
				float nowDist = (nearestToLine - blockCenter).GetLengthSquared();
				if (nowDist < radius * radius) {
					m_blocks[blockIndex].m_blockID = BlockDef::GetBlockDefByName("air");
					m_blocks[blockIndex].SetFlagsByDef();

					if (nowDist < minDist) {
						minDist = nowDist;
						index = blockIndex;
					}
				}
				/*
				if ((blockCenter - nowPosVec3).GetLengthSquared() < 100.f) {
					m_blocks[blockIndex].m_blockID = BlockDef::GetBlockDefByName("air");
					m_blocks[blockIndex].SetFlagsByDef();
				}*/
			}
			lastPos = nowPos;
		}
	}
	// Each cave's RNG seed will be based on raw noide of its ckunk coords
	// Generate each cave , carving blocks as it goes; only MY blocks will actually be carved
}

void Chunk::AddHouse()
{
	//float neighbors[8] = { 999.f };
	int dirtX[3] = {}, dirtY[3] = {};
	dirtX[0] = 1, dirtY[0] = 1;
	dirtX[1] = -1, dirtY[1] = -1;
	dirtX[2] = 0, dirtY[2] = 0;
	for (int xIndex = 0; xIndex < 3; xIndex++) {
		for (int yIndex = 0; yIndex < 3; yIndex++) {
			int offsetX = dirtX[xIndex];
			int offsetY = dirtY[yIndex];
			int nowX = m_chunkCoords.x + offsetX;
			int nowY = m_chunkCoords.y + offsetY;
			if (IsHouseChunk(nowX, nowY, m_world->m_seed)) {
				RandomNumberGenerator rng;
				rng.m_seed = nowX + nowY * 1256347;
				int spawnX = rng.RollRandomIntInRange(0, 15);
				int spawnY = rng.RollRandomIntInRange(0, 15);
				int blockX = spawnX + nowX * LOCAL_X_SIZE;
				int blockY = spawnY + nowY * LOCAL_Y_SIZE;
				int z = GetTerrainHeightZ(blockX, blockY);
				if (GetTemperature(blockX, blockY) < 0.4f) {
					SpawnBlockTemplateAtCoords("pinkHouse",
						IntVec3(blockX - m_chunkCoords.x * LOCAL_X_SIZE, blockY - m_chunkCoords.y * LOCAL_Y_SIZE, z));
				}
				else
				SpawnBlockTemplateAtCoords("house", 
					IntVec3(blockX - m_chunkCoords.x * LOCAL_X_SIZE, blockY - m_chunkCoords.y * LOCAL_Y_SIZE, z));
			}
		}
	}
}

bool Chunk::IsHouseChunk(int globalX, int globalY, unsigned int seed)
{
	
	float now = Get2dNoiseZeroToOne(globalX, globalY, seed);
	if (now < 0.999f)
		return false;
	float neighbors[8] = { 999.f };
	int dirtX[3] = {}, dirtY[3] = {};
	dirtX[0] = 1, dirtY[0] = 1;
	dirtX[1] = -1, dirtY[1] = -1;
	dirtX[2] = 0, dirtY[2] = 0;

	int neighborIndex = 0;
	for (int xIndex = 0; xIndex < 3; xIndex++) {
		for (int yIndex = 0; yIndex < 3; yIndex++) {
			int offsetX = dirtX[xIndex];
			int offsetY = dirtY[yIndex];
			if (offsetX != 0 && offsetY != 0) {
				neighbors[neighborIndex] = Get2dNoiseZeroToOne(globalX + offsetX, globalY + offsetY, seed);
				neighborIndex++;
			}

		}
	}

	for (int index = 0; index < 8; index++) {
		if (now <= neighbors[index]) {
			return false;
		}
	}
	return true;
}


AABB3 Chunk::GetLocalBoundsByIndex(int index) const
{
	IntVec3 coords = GetBlockCoordsByIndex(index);
	AABB3 bound = AABB3(Vec3(float(coords.x), float(coords.y), float(coords.z)), Vec3(float(coords.x + 1), float(coords.y + 1), float(coords.z + 1)));
	return bound;
}

AABB3 Chunk::GetWorldBoundsByIndex(int index) const
{
	AABB3 localBound = GetLocalBoundsByIndex(index);
	localBound.m_mins += m_worldBounds.m_mins;
	localBound.m_maxs += m_worldBounds.m_mins;
	return localBound;
}

IntVec3 Chunk::GetBlockCoordsByIndex(int index) const
{
	int x = index & (LOCAL_X_SIZE - 1);
	int y = (index >> X_BIT_SIZE) & (LOCAL_Y_SIZE - 1);
	int z = (index >> (X_BIT_SIZE + Y_BIT_SIZE));
	return IntVec3(x, y, z);
}

Vec3 Chunk::GetBlockWorldCenterByIndex(int index) const
{	
	return GetWorldBoundsByIndex(index).GetCenter();
}

int Chunk::GetBlockIndexByCoords(IntVec3 coords) const
{
	int x = coords.x;
	int y = coords.y << X_BIT_SIZE;
	int z = coords.z << (X_BIT_SIZE + Y_BIT_SIZE);
	int index = x | y | z;
	//int index = coords.x || (coords.y << X_BIT_SIZE) || (coords.z << (X_BIT_SIZE + Y_BIT_SIZE));
	return index;
}

bool Chunk::IsCoordsInBounds(const IntVec3& coords) const
{
	if (coords.x >= 0 && coords.x < LOCAL_X_SIZE) {
		if (coords.y >= 0 && coords.y < LOCAL_Y_SIZE) {
			if (coords.z >= 0 && coords.z < LOCAL_Z_SIZE) {
				return true;
			}
		}
	}
	return false;
}
bool Chunk::IsBlockOnBorder(int index) const
{
	IntVec3 coord = GetBlockCoordsByIndex(index);
	if (coord.x == 0 || coord.x == LOCAL_X_SIZE - 1)
		return true;
	if (coord.y == 0 || coord.y == LOCAL_Y_SIZE - 1)
		return true;
	if (coord.z == 0 || coord.z == LOCAL_Z_SIZE - 1)
		return true;
	return false;
}
bool Chunk::IsHavingAllNeighbors() const
{
	if (m_eastNeighbor == nullptr)
		return false;
	if (m_westNeighbor == nullptr)
		return false;
	if (m_northNeighbor == nullptr)
		return false;
	if (m_southNeighbor == nullptr)
		return false;
	return true;
}
bool Chunk::IsTreeColumn(int globalX, int globalY, unsigned int seed)
{
	float now = Get2dNoiseZeroToOne(globalX, globalY, seed);
	float neighbors[24] = {999.f};
	int dirtX[5] = {}, dirtY[5] = {};
	dirtX[0] = -2, dirtY[0] = -2;
	dirtX[1] = -1, dirtY[1] = -1;
	dirtX[2] = 0, dirtY[2] = 0;
	dirtX[3] = 1, dirtY[3] = 1;
	dirtX[4] = 2, dirtY[4] = 2;
	int neighborIndex = 0;
	for (int xIndex = 0; xIndex < 5; xIndex++) {
		for (int yIndex = 0; yIndex < 5; yIndex++) {
			int offsetX = dirtX[xIndex];
			int offsetY = dirtY[yIndex];
			if (offsetX != 0 && offsetY != 0) {
				neighbors[neighborIndex] = Get2dNoiseZeroToOne(globalX + offsetX, globalY + offsetY, seed);
				neighborIndex++;
			}

		}
	}

	for (int index = 0; index < 24; index++) {
		if (now <= neighbors[index]) {
			return false;
		}
	}
	return true;
}
bool Chunk::IsLeafColumn(int globalX, int globalY, unsigned int seed)
{
	int localX = globalX - m_chunkCoords.x * LOCAL_X_SIZE;
	int localY = globalY - m_chunkCoords.y * LOCAL_Y_SIZE;
	if (IsTreeColumn(globalX, globalY, seed))
		return false;
	int dirtX[5] = {0}, dirtY[5] = { 0 };
	dirtX[0] = -2, dirtY[0] = -2;
	dirtX[1] = -1, dirtY[1] = -1;
	dirtX[2] = 0, dirtY[2] = 0;
	dirtX[3] = 1, dirtY[3] = 1;
	dirtX[4] = 2, dirtY[4] = 2;

	for (int xIndex = 0; xIndex < 5; xIndex++) {
		for (int yIndex = 0; yIndex < 5; yIndex++) {
			int offsetX = dirtX[xIndex];
			int offsetY = dirtY[yIndex];
			if (offsetX != 0 && offsetY != 0) {
				int nowX = globalX + offsetX;
				int nowY = globalY + offsetY;
			
				if (IsTreeColumn(nowX, nowY, seed)) {
					if (IsATreeFinally(nowX, nowY)) {
						int nowLX = localX + offsetX;
						int nowLY = localY + offsetY;
						float humidity = GetHumidity(nowX, nowY);
						float temperature = GetTemperature(nowX, nowY);
						int terrainHeightZ = GetTerrainHeightZ(nowX, nowY);
						int zIndex = terrainHeightZ + 1;
						if (humidity < 0.2f)
							SpawnBlockTemplateAtCoords("cactus", IntVec3(nowLX, nowLY, zIndex));
						else if (temperature < 0.4f) {
							SpawnBlockTemplateAtCoords("spruceTree", IntVec3(nowLX, nowLY, zIndex));
						}
						else {
							SpawnBlockTemplateAtCoords("oakTree", IntVec3(nowLX, nowLY, zIndex));
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}
float Chunk::GetHumidity(int globalX, int globalY)
{
	float humidity = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 200.f, 3, 0.5f, 2.f, m_world->m_humiditySeed);
	return humidity;
}
float Chunk::GetTemperature(int globalX, int globalY)
{
	return  0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 5, 0.5f, 2.f, m_world->m_temperatureSeed);
}
int Chunk::GetTerrainHeightZ(int globalX, int globalY)
{
	float hilliness = SmoothStep3(0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 3, 0.5f, 2.f, m_world->m_hillinessSeed));
	//hilliness = SmoothStep3(hilliness);
	int hillheight = RoundDownToInt( 74 * hilliness);
	int terrainHeightZ = 62 + int(fabsf(Compute2dPerlinNoise(float(globalX), float(globalY), 200.f, 5, 0.5f, 2.f, m_world->m_terrainHeightSeed)) * hillheight);

	float oceanness = GetClamped(Compute2dPerlinNoise(float(globalX), float(globalY), 400.f, 3, 0.5f, 2.f, m_world->m_oceannessSeed), 0.f, 0.5f);
	oceanness = RangeMap(oceanness, 0.f, 0.5f, 0.f, 1.f);
	oceanness = SmoothStep3(oceanness);
	int maxOceanDepth = 40;
	int nowOceanDepth = RoundDownToInt(maxOceanDepth * oceanness);
	terrainHeightZ -= nowOceanDepth;
	return terrainHeightZ;
}
bool Chunk::IsATreeFinally(int globalX, int globalY)
{
	float treeNoise = Get2dNoiseZeroToOne(globalX, globalY, m_world->m_treeSeed);

	float terrainHeightZ = (float)GetTerrainHeightZ(globalX, globalY);
	
	float forestness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 800.f, 3, 0.5f, 2.f, m_world->m_forestnessSeed);

	float treeThreshold = RangeMapClamped(forestness, 0.5f, 1.f, 1.f, 0.9f);

	bool isTree = false;

	if (treeNoise > treeThreshold && terrainHeightZ > LOCAL_Z_SIZE / 2)
		isTree = true;
	return isTree;
}
IntVec3 Chunk::GetLocalBlockCoordsByPosition(const Vec3& position) const
{
	Vec3 localPos = position - m_worldBounds.m_mins;
	int x = RoundDownToInt(localPos.x);
	int y = RoundDownToInt(localPos.y);
	int z = RoundDownToInt(localPos.z);
	return IntVec3(x, y, z);
}
IntVec3 Chunk::GetWorldBlockCoordsByPosition(const Vec3& position) const
{
	Vec3 worldPos = position;
	int x = RoundDownToInt(worldPos.x);
	int y = RoundDownToInt(worldPos.y);
	int z = RoundDownToInt(worldPos.z);
	return IntVec3(x, y, z);
}
IntVec3 Chunk::GetFirstOneNotAir(const Vec3& position) const
{
	IntVec3 nowBlock = GetLocalBlockCoordsByPosition(position);
	for (int zIndex = nowBlock.z; zIndex >= 0; zIndex--) {
		int index = GetBlockIndexByCoords(IntVec3(nowBlock.x, nowBlock.y, zIndex));
		BlockDef* blockDef = BlockDef::s_blockDefs[m_blocks[index].m_blockID];
		if (blockDef->m_name != "air")
		{
			return IntVec3(nowBlock.x, nowBlock.y, zIndex);
		}
	}
	return IntVec3(nowBlock.x, nowBlock.y, 0);
}
void Chunk::Render() const
{
	g_theRenderer->BindTexture(&g_basicSprite->GetTexture());
	//g_theRenderer->DrawVertexArray((int)m_cpuMesh.size(), m_cpuMesh.data());
	if (m_gpuMesh != nullptr)
		g_theRenderer->DrawVertexBuffer(m_gpuMesh, (int)m_cpuMesh.size());
	g_theRenderer->BindTexture(nullptr);

	if (m_isDebug) {
		std::vector<Vertex_PCU> debugFrameVerts;
		AddVertsForAABBWireframe3D(debugFrameVerts, m_worldBounds, 0.05f);
		g_theRenderer->DrawVertexArray((int)debugFrameVerts.size(), debugFrameVerts.data());
	}
}

void Chunk::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	if (m_isMeshDataDirty && IsHavingAllNeighbors() && m_world->m_rebuildMeshTimes < 2) {
		RebuildChunkMeshVerts();
	}
}

void Chunk::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1)) {
		m_isDebug = !m_isDebug;
	}
	
}

void Chunk::AddVertsForBlock(std::vector<Vertex_PCU> &verts, int index)
{
	BlockDef* blockDef = BlockDef::s_blockDefs[m_blocks[index].m_blockID];
	BlockIterator blockIt = BlockIterator(this, index);
	if (!blockDef->m_isOpaque || !blockDef->m_isVisible)
		return;
	AABB3 bounds = GetWorldBoundsByIndex(index);
	IntVec3 coords = GetBlockCoordsByIndex(index);
	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;
	AABB2 sideUV = g_basicSprite->GetSpriteUVs(blockDef->m_sidesSprite);
	AABB2 topUV = g_basicSprite->GetSpriteUVs(blockDef->m_topSprite);
	AABB2 bottomUV = g_basicSprite->GetSpriteUVs(blockDef->m_bottomSprite);
	Rgba8 color;
	
	// +x
	//if (IsCoordsInBounds(IntVec3(coords.x + 1, coords.y, coords.z))) {
		BlockIterator east = blockIt.GetEastNeighbor();
		
			//GetBlockIndexByCoords(IntVec3(coords.x + 1, coords.y, coords.z));
		if (!east.IsEmpty()) {
			BlockDef* next_bd = BlockDef::s_blockDefs[east.GetBlock()->m_blockID];
			if (!next_bd->m_isOpaque) {
				color = blockIt.GetNeighborLight(FaceDirt::EAST);
				color.b = 127;
				AddVertsForQuad3D(verts, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), color, sideUV);

			}
		}
		
	//}
	//else
	//{
		//color = blockIt.GetNeighborLight(NeighborDirt::EAST);
		//color.b = 127;
		//AddVertsForQuad3D(verts, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), color, sideUV);
	//}
	// -x
	//if (IsCoordsInBounds(IntVec3(coords.x - 1, coords.y, coords.z))) {
		//next = GetBlockIndexByCoords(IntVec3(coords.x - 1, coords.y, coords.z));
		BlockIterator west = blockIt.GetWestNeighbor();
		if (!west.IsEmpty()) {
			BlockDef* next_bd = BlockDef::s_blockDefs[west.GetBlock()->m_blockID];
			if (!next_bd->m_isOpaque) {
				color = blockIt.GetNeighborLight(FaceDirt::WEST);
				color.b = 127;
				AddVertsForQuad3D(verts, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), color, sideUV);
			}
		}
		
	//}
	//else
	//{
		//color = blockIt.GetNeighborLight(NeighborDirt::WEST);
		//color.b = 127;
	//	AddVertsForQuad3D(verts, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), color, sideUV);
	//}
	// +y
	//if (IsCoordsInBounds(IntVec3(coords.x, coords.y + 1, coords.z))) {
		BlockIterator north = blockIt.GetNorthNeighbor();
		if (!north.IsEmpty()) {
			BlockDef* next_bd = BlockDef::s_blockDefs[north.GetBlock()->m_blockID];
			if (!next_bd->m_isOpaque) {
				color = blockIt.GetNeighborLight(FaceDirt::NORTH);
				color.b = 127;
				AddVertsForQuad3D(verts, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), color, sideUV);
			}

		}
	// -y
	//if (IsCoordsInBounds(IntVec3(coords.x, coords.y - 1, coords.z))) {
		BlockIterator south = blockIt.GetSouthNeighbor();
		if (!south.IsEmpty()) {
			BlockDef* next_bd = BlockDef::s_blockDefs[south.GetBlock()->m_blockID];
			if (!next_bd->m_isOpaque) {
				color = blockIt.GetNeighborLight(FaceDirt::SOUTH);
				color.b = 127;
				AddVertsForQuad3D(verts, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), color, sideUV);
			}
		}
		
	//}
	//else
	//{
		//color = blockIt.GetNeighborLight(NeighborDirt::SOUTH);
		//color.b = 127;
	//	AddVertsForQuad3D(verts, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), color, sideUV);
	//}
	// +z
	if (IsCoordsInBounds(IntVec3(coords.x, coords.y, coords.z + 1))) {
		int nextZ = GetBlockIndexByCoords(IntVec3(coords.x, coords.y, coords.z + 1));
		BlockDef* next_bdZ = BlockDef::s_blockDefs[m_blocks[nextZ].m_blockID];
		if (!next_bdZ->m_isOpaque) {
			color = blockIt.GetNeighborLight(FaceDirt::UP);
			color.b = 127;
			AddVertsForQuad3D(verts, Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, minY, maxZ), color, topUV);
		}
	}
	else
	{
		//color = blockIt.GetNeighborLight(NeighborDirt::UP);
		//color.b = 127;
		AddVertsForQuad3D(verts, Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, minY, maxZ), color, topUV);
	}
	// -z
	if (IsCoordsInBounds(IntVec3(coords.x, coords.y, coords.z - 1))) {
		int nextZ = GetBlockIndexByCoords(IntVec3(coords.x, coords.y, coords.z - 1));
		BlockDef* next_bdZ = BlockDef::s_blockDefs[m_blocks[nextZ].m_blockID];
		if (!next_bdZ->m_isOpaque) {
			color = blockIt.GetNeighborLight(FaceDirt::DOWN);
			color.b = 127;
			AddVertsForQuad3D(verts, Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), color, bottomUV);
		}
	}
	else
	{
		//color = blockIt.GetNeighborLight(NeighborDirt::DOWN);
		//color.b = 127;
		AddVertsForQuad3D(verts, Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), color, bottomUV);
	}

}


