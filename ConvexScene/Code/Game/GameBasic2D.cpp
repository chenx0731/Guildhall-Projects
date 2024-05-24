#define UNUSED(x) (void)(x);
#include "Game/GameBasic2D.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/TestBinaryFileSaveLoadCode.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <queue>
#include <algorithm>

enum ChunkType : unsigned char {
	CHUNKTYPE_SCENEINFO = 0x01,
	CHUNKTYPE_CONVEXPOLY = 0x02,
	CHUNKTYPE_CONVEXHULL = 0x80,
	CHUNKTYPE_BOUNDING_DISCS = 0x81,
	CHUNKTYPE_DISC2_TREE = 0x8A,
};

unsigned char const FOURCC_S_0 = 'G';
unsigned char const FOURCC_S_1 = 'H';
unsigned char const FOURCC_S_2 = 'C';
unsigned char const FOURCC_S_3 = 'S';
unsigned char const FOURCC_E_0 = 'E';
unsigned char const FOURCC_E_1 = 'N';
unsigned char const FOURCC_E_2 = 'D';
unsigned char const FOURCC_E_3 = 'H';
unsigned char const COHORT_ID = 32;
unsigned char const MAJOR_FILE_V = 1;
unsigned char const MINOR_FILE_V = 1;
unsigned char const ENDIANNESS = (unsigned char)EndianMode::LITTLE;

unsigned char const CHUNK_FOURCC_S_0 = 'G';
unsigned char const CHUNK_FOURCC_S_1 = 'H';
unsigned char const CHUNK_FOURCC_S_2 = 'C';
unsigned char const CHUNK_FOURCC_S_3 = 'K';

unsigned char const CHUNK_FOURCC_E_0 = 'E';
unsigned char const CHUNK_FOURCC_E_1 = 'N';
unsigned char const CHUNK_FOURCC_E_2 = 'D';
unsigned char const CHUNK_FOURCC_E_3 = 'C';

unsigned char const TOC_FOURCC_S_0 = 'G';
unsigned char const TOC_FOURCC_S_1 = 'H';
unsigned char const TOC_FOURCC_S_2 = 'T';
unsigned char const TOC_FOURCC_S_3 = 'C';

unsigned char const TOC_NUM_OF_CHUNK = 3;

unsigned char const TOC_FOURCC_E_0 = 'E';
unsigned char const TOC_FOURCC_E_1 = 'N';
unsigned char const TOC_FOURCC_E_2 = 'D';
unsigned char const TOC_FOURCC_E_3 = 'T';

uint32_t const SCENEINFO_DATA_SIZE = 20;

void CheckFourCC(BufferParser& bufParser, unsigned char* fourCC)
{
	unsigned char chunk_fourCC_S_0 = bufParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_0 == fourCC[0], "FourCC doesn't match: 1");
	unsigned char chunk_fourCC_S_1 = bufParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_1 == fourCC[1], "FourCC doesn't match: 2");
	unsigned char chunk_fourCC_S_2 = bufParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_2 == fourCC[2], "FourCC doesn't match: 3");
	unsigned char chunk_fourCC_S_3 = bufParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_3 == fourCC[3], "FourCC doesn't match: 4");
}

void AppendSceneInfoChunk(BufferWriter& bufferWriter, BufferWriter& tocWriter)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	uint32_t start = bufferWriter.GetBufferSize();

	bufferWriter.AppendByte(CHUNK_FOURCC_S_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_3);

	bufferWriter.AppendByte(CHUNKTYPE_SCENEINFO);
	bufferWriter.AppendByte(ENDIANNESS);
	bufferWriter.SetEndianMode((EndianMode)ENDIANNESS);

	bufferWriter.AppendUint32(SCENEINFO_DATA_SIZE);
	
	bufferWriter.AppendAABB2(game->m_worldBound);

	bufferWriter.AppendUint32((uint32_t)game->m_numOfObjects);

	bufferWriter.AppendByte(CHUNK_FOURCC_E_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_3);

	uint32_t size = bufferWriter.GetBufferSize() - start;

	tocWriter.AppendByte(CHUNKTYPE_SCENEINFO);
	tocWriter.AppendUint32(start);
	tocWriter.AppendUint32(size);
}

void AppendConvexPolyInfoChunk(BufferWriter& bufferWriter, BufferWriter& tocWriter)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	uint32_t locationOfChunk = bufferWriter.GetBufferSize();
	bufferWriter.AppendByte(CHUNK_FOURCC_S_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_3);

	bufferWriter.AppendByte(CHUNKTYPE_CONVEXPOLY);
	bufferWriter.AppendByte(ENDIANNESS);
	bufferWriter.SetEndianMode((EndianMode)ENDIANNESS);

	uint32_t locationOfDataSize = bufferWriter.GetBufferSize();
	bufferWriter.AppendUint32(SCENEINFO_DATA_SIZE);

	bufferWriter.AppendUint32(game->m_numOfObjects);

	for (int i = 0; i < game->m_numOfObjects; i++) {
		game->m_objects[i].WritePolyIntoBuffer(bufferWriter);
	}
	uint32_t dataSize = bufferWriter.GetBufferSize() - locationOfDataSize;
	if (bufferWriter.m_isEndiannessReversed)
		Reverse4BytesInPlace(&dataSize);
	bufferWriter.ModifyUint32ValueByLocation(locationOfDataSize, &dataSize, sizeof(uint32_t));

	bufferWriter.AppendByte(CHUNK_FOURCC_E_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_3);

	uint32_t chunkSize = bufferWriter.GetBufferSize() - locationOfChunk;

	tocWriter.AppendByte(CHUNKTYPE_CONVEXPOLY);
	tocWriter.AppendUint32(locationOfChunk);
	tocWriter.AppendUint32(chunkSize);
}

void AppendBoundingDiscInfoChunk(BufferWriter& bufferWriter, BufferWriter& tocWriter)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	uint32_t locationOfChunk = bufferWriter.GetBufferSize();
	bufferWriter.AppendByte(CHUNK_FOURCC_S_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_3);

	bufferWriter.AppendByte(CHUNKTYPE_BOUNDING_DISCS);
	bufferWriter.AppendByte(ENDIANNESS);
	bufferWriter.SetEndianMode((EndianMode)ENDIANNESS);

	uint32_t locationOfDataSize = bufferWriter.GetBufferSize();
	bufferWriter.AppendUint32(SCENEINFO_DATA_SIZE);

	bufferWriter.AppendUint32(game->m_numOfObjects);

	for (int i = 0; i < game->m_numOfObjects; i++) {
		game->m_objects[i].WriteDiscIntoBuffer(bufferWriter);
	}
	uint32_t dataSize = bufferWriter.GetBufferSize() - locationOfDataSize;
	if (bufferWriter.m_isEndiannessReversed)
		Reverse4BytesInPlace(&dataSize);
	bufferWriter.ModifyUint32ValueByLocation(locationOfDataSize, &dataSize, sizeof(uint32_t));

	bufferWriter.AppendByte(CHUNK_FOURCC_E_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_3);

	uint32_t chunkSize = bufferWriter.GetBufferSize() - locationOfChunk;

	tocWriter.AppendByte(CHUNKTYPE_BOUNDING_DISCS);
	tocWriter.AppendUint32(locationOfChunk);
	tocWriter.AppendUint32(chunkSize);
}

void AppendDisc2TreeChunk(BufferWriter& bufferWriter, BufferWriter& tocWriter)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	uint32_t locationOfChunk = bufferWriter.GetBufferSize();
	bufferWriter.AppendByte(CHUNK_FOURCC_S_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_S_3);

	bufferWriter.AppendByte(CHUNKTYPE_BOUNDING_DISCS);
	bufferWriter.AppendByte(ENDIANNESS);
	bufferWriter.SetEndianMode((EndianMode)ENDIANNESS);

	uint32_t locationOfDataSize = bufferWriter.GetBufferSize();
	bufferWriter.AppendUint32(SCENEINFO_DATA_SIZE);

	bufferWriter.AppendUint32((uint32_t)game->m_disc2Tree.size());

	for (int i = 0; i < (int)game->m_disc2Tree.size(); i++) {
		/*
		bufferWriter.AppendVec2(game->m_disc2Tree[i].m_discCenter);
		bufferWriter.AppendFloat(game->m_disc2Tree[i].m_discRadius);
		uint32_t numOfEntity = (uint32_t)game->m_disc2Tree[i].m_entityIndex.size();
		bufferWriter.AppendUint32(numOfEntity);
		for (int j = 0; j < numOfEntity; j++) {
			bufferWriter.AppendInt32(game->m_disc2Tree[i].m_entityIndex[j]);
		}
		bufferWriter.AppendInt32(game->m_disc2Tree[i].m_leftChild);
		bufferWriter.AppendInt32(game->m_disc2Tree[i].m_rightChild);*/
		game->m_disc2Tree[i].WriteIntoBuffer(bufferWriter);
	}

	uint32_t dataSize = bufferWriter.GetBufferSize() - locationOfDataSize;
	if (bufferWriter.m_isEndiannessReversed)
		Reverse4BytesInPlace(&dataSize);
	bufferWriter.ModifyUint32ValueByLocation(locationOfDataSize, &dataSize, sizeof(uint32_t));

	bufferWriter.AppendByte(CHUNK_FOURCC_E_0);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_1);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_2);
	bufferWriter.AppendByte(CHUNK_FOURCC_E_3);

	uint32_t chunkSize = bufferWriter.GetBufferSize() - locationOfChunk;

	tocWriter.AppendByte(CHUNKTYPE_BOUNDING_DISCS);
	tocWriter.AppendUint32(locationOfChunk);
	tocWriter.AppendUint32(chunkSize);
}

void ParseSceneInfoChunk(BufferParser& chunkParser)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];
	
	unsigned char chunk_fourCC_S_0 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_0 == CHUNK_FOURCC_S_0, "Chunk header format doesn't match: 1");
	unsigned char chunk_fourCC_S_1 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_1 == CHUNK_FOURCC_S_1, "Chunk header format doesn't match: 2");
	unsigned char chunk_fourCC_S_2 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_2 == CHUNK_FOURCC_S_2, "Chunk header format doesn't match: 3");
	unsigned char chunk_fourCC_S_3 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_3 == CHUNK_FOURCC_S_3, "Chunk header format doesn't match: 4");

	unsigned char chunk_type = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_type == CHUNKTYPE_SCENEINFO, "Chunk type doesn't match");

	unsigned char endianness = chunkParser.ParseByte();
	chunkParser.SetEndianMode((EndianMode)endianness);

	uint32_t size = chunkParser.ParseUint32();
	UNUSED(size);

	game->m_worldBound = chunkParser.ParseAABB2();
	game->m_worldCamera.SetOrthoView(game->m_worldBound.m_mins, game->m_worldBound.m_maxs);
	game->m_numOfObjects = (int)chunkParser.ParseUint32();

	unsigned char endOfChunk[4];
	endOfChunk[0] = CHUNK_FOURCC_E_0;
	endOfChunk[1] = CHUNK_FOURCC_E_1;
	endOfChunk[2] = CHUNK_FOURCC_E_2;
	endOfChunk[3] = CHUNK_FOURCC_E_3;
	CheckFourCC(chunkParser, endOfChunk);
}

void ParseConvexPolyChunk(BufferParser& chunkParser)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	unsigned char chunk_fourCC_S_0 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_0 == CHUNK_FOURCC_S_0, "Chunk header format doesn't match: 1");
	unsigned char chunk_fourCC_S_1 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_1 == CHUNK_FOURCC_S_1, "Chunk header format doesn't match: 2");
	unsigned char chunk_fourCC_S_2 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_2 == CHUNK_FOURCC_S_2, "Chunk header format doesn't match: 3");
	unsigned char chunk_fourCC_S_3 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_3 == CHUNK_FOURCC_S_3, "Chunk header format doesn't match: 4");

	unsigned char chunk_type = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_type == CHUNKTYPE_CONVEXPOLY, "Chunk type doesn't match");

	unsigned char endianness = chunkParser.ParseByte();
	chunkParser.SetEndianMode((EndianMode)endianness);

	uint32_t size = chunkParser.ParseUint32();
	UNUSED(size);

	uint32_t num = chunkParser.ParseUint32();
	game->m_objects.clear();
	//if (num > (uint32_t)game->m_objects.size())
	game->m_objects.resize(num);
	game->m_numOfObjects = num;
	for (uint32_t i = 0; i < num; i++)
	{
		game->m_objects[i].ParsePolyFromBuffer(chunkParser);
	}
	game->m_isTreeUpdated = false;

	unsigned char endOfChunk[4];
	endOfChunk[0] = CHUNK_FOURCC_E_0;
	endOfChunk[1] = CHUNK_FOURCC_E_1;
	endOfChunk[2] = CHUNK_FOURCC_E_2;
	endOfChunk[3] = CHUNK_FOURCC_E_3;
	CheckFourCC(chunkParser, endOfChunk);
}

void ParseBoundingDiscChunk(BufferParser& chunkParser)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	unsigned char chunk_fourCC_S_0 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_0 == CHUNK_FOURCC_S_0, "Chunk header format doesn't match: 1");
	unsigned char chunk_fourCC_S_1 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_1 == CHUNK_FOURCC_S_1, "Chunk header format doesn't match: 2");
	unsigned char chunk_fourCC_S_2 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_2 == CHUNK_FOURCC_S_2, "Chunk header format doesn't match: 3");
	unsigned char chunk_fourCC_S_3 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_3 == CHUNK_FOURCC_S_3, "Chunk header format doesn't match: 4");

	unsigned char chunk_type = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_type == CHUNKTYPE_BOUNDING_DISCS, "Chunk type doesn't match");

	unsigned char endianness = chunkParser.ParseByte();
	chunkParser.SetEndianMode((EndianMode)endianness);

	uint32_t size = chunkParser.ParseUint32();
	UNUSED(size);

	uint32_t num = chunkParser.ParseUint32();

	if (num > (uint32_t)game->m_objects.size())
		game->m_objects.resize(num);
	game->m_numOfObjects = num;
	for (uint32_t i = 0; i < num; i++)
	{
		game->m_objects[i].ParseDiscFromBuffer(chunkParser);
	}
	unsigned char endOfChunk[4];
	endOfChunk[0] = CHUNK_FOURCC_E_0;
	endOfChunk[1] = CHUNK_FOURCC_E_1;
	endOfChunk[2] = CHUNK_FOURCC_E_2;
	endOfChunk[3] = CHUNK_FOURCC_E_3;
	CheckFourCC(chunkParser, endOfChunk);
}

void ParseDisc2TreeChunk(BufferParser& chunkParser)
{
	GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];

	unsigned char chunk_fourCC_S_0 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_0 == CHUNK_FOURCC_S_0, "Chunk header format doesn't match: 1");
	unsigned char chunk_fourCC_S_1 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_1 == CHUNK_FOURCC_S_1, "Chunk header format doesn't match: 2");
	unsigned char chunk_fourCC_S_2 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_2 == CHUNK_FOURCC_S_2, "Chunk header format doesn't match: 3");
	unsigned char chunk_fourCC_S_3 = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_fourCC_S_3 == CHUNK_FOURCC_S_3, "Chunk header format doesn't match: 4");

	unsigned char chunk_type = chunkParser.ParseByte();
	GUARANTEE_OR_DIE(chunk_type == CHUNKTYPE_BOUNDING_DISCS, "Chunk type doesn't match");

	unsigned char endianness = chunkParser.ParseByte();
	chunkParser.SetEndianMode((EndianMode)endianness);

	uint32_t size = chunkParser.ParseUint32();
	UNUSED(size);

	uint32_t num = chunkParser.ParseUint32();

	if (num > (uint32_t)game->m_disc2Tree.size())
		game->m_disc2Tree.resize(num);

	for (uint32_t i = 0; i < num; i++)
	{
		//game->m_objects[i].ParseDiscFromBuffer(chunkParser);
		game->m_disc2Tree[i].ParseFromBuffer(chunkParser);
	}
	game->m_isTreeUpdated = true;

	unsigned char endOfChunk[4];
	endOfChunk[0] = CHUNK_FOURCC_E_0;
	endOfChunk[1] = CHUNK_FOURCC_E_1;
	endOfChunk[2] = CHUNK_FOURCC_E_2;
	endOfChunk[3] = CHUNK_FOURCC_E_3;
	CheckFourCC(chunkParser, endOfChunk);
}

void ParseChunkByType(ChunkType chunkType, BufferParser& chunckParser)
{
	switch (chunkType)
	{
	case CHUNKTYPE_SCENEINFO:
	{
		ParseSceneInfoChunk(chunckParser);
		break;
	}
	case CHUNKTYPE_CONVEXPOLY: 
	{
		ParseConvexPolyChunk(chunckParser);
		break;
	}
	case CHUNKTYPE_CONVEXHULL:
	{
		break;
	}
	case CHUNKTYPE_BOUNDING_DISCS:
	{
		ParseBoundingDiscChunk(chunckParser);
		break;
	}
	case CHUNKTYPE_DISC2_TREE:
	{
		ParseDisc2TreeChunk(chunckParser);
		break;
	}
	default:
		break;
	}
}

bool SaveSceneToFile(EventArgs& args)
{
	//GameBasic2D* game = (GameBasic2D*)g_theGames[(int)GameStatus::BASIC_2D];
	// header
	std::vector<uint8_t> fileheader;
	std::vector<uint8_t> tableOfContent;
	BufferWriter headerWriter = BufferWriter(fileheader);
	BufferWriter tocWriter = BufferWriter(tableOfContent);
	headerWriter.AppendByte(FOURCC_S_0);
	headerWriter.AppendByte(FOURCC_S_1);
	headerWriter.AppendByte(FOURCC_S_2);
	headerWriter.AppendByte(FOURCC_S_3);

	headerWriter.AppendByte(COHORT_ID);

	headerWriter.AppendByte(MAJOR_FILE_V);

	headerWriter.AppendByte(MINOR_FILE_V);

	headerWriter.AppendByte(ENDIANNESS);
	headerWriter.SetEndianMode((EndianMode)ENDIANNESS);

	// Location of ToC
	uint32_t locationOfTOC = headerWriter.GetBufferSize();
	headerWriter.AppendUint32(0);

	//uint32_t *const locationOfTOC = reinterpret_cast<uint32_t *const>(&fileheader[8]);

	headerWriter.AppendByte(FOURCC_E_0);
	headerWriter.AppendByte(FOURCC_E_1);
	headerWriter.AppendByte(FOURCC_E_2);
	headerWriter.AppendByte(FOURCC_E_3);

	size_t curSize = fileheader.size();
	UNUSED(curSize);

	tocWriter.SetEndianMode((EndianMode)ENDIANNESS);
	tocWriter.AppendByte(TOC_FOURCC_S_0);
	tocWriter.AppendByte(TOC_FOURCC_S_1);
	tocWriter.AppendByte(TOC_FOURCC_S_2);
	tocWriter.AppendByte(TOC_FOURCC_S_3);

	tocWriter.AppendByte(TOC_NUM_OF_CHUNK);

	AppendSceneInfoChunk(headerWriter, tocWriter);
	AppendConvexPolyInfoChunk(headerWriter, tocWriter);
	AppendBoundingDiscInfoChunk(headerWriter, tocWriter);
	AppendDisc2TreeChunk(headerWriter, tocWriter);
	// polys
	// discs

	uint32_t startOfTOC = headerWriter.GetBufferSize();

	//*locationOfTOC = startOfTOC;
	if (headerWriter.m_isEndiannessReversed)
		Reverse4BytesInPlace(&startOfTOC);
	//memcpy(&fileheader[8], &startOfTOC, sizeof(uint32_t));
	headerWriter.ModifyUint32ValueByLocation(locationOfTOC, &startOfTOC, sizeof(uint32_t));

	tocWriter.AppendByte(TOC_FOURCC_E_0);
	tocWriter.AppendByte(TOC_FOURCC_E_1);
	tocWriter.AppendByte(TOC_FOURCC_E_2);
	tocWriter.AppendByte(TOC_FOURCC_E_3);

	fileheader.insert(fileheader.end(), tableOfContent.begin(), tableOfContent.end());

	std::string file = args.GetValue("file", "");

	WriteToFile(fileheader, file);

	return true;
}

bool LoadSceneFromFile(EventArgs& args)
{
	std::string file = args.GetValue("file", "");
	std::vector<uint8_t> buffer;
	if (IsFileExist(file)) {
		FileReadToBuffer(buffer, file);
		BufferParser headerParser = BufferParser(buffer.data(), 16);

		unsigned char fourCC_S_0 = headerParser.ParseByte();
		GUARANTEE_OR_DIE(fourCC_S_0 == FOURCC_S_0, "File format doesn't match: 1");
		unsigned char fourCC_S_1 = headerParser.ParseByte();
		GUARANTEE_OR_DIE(fourCC_S_1 == FOURCC_S_1, "File format doesn't match: 2");
		unsigned char fourCC_S_2 = headerParser.ParseByte();
		GUARANTEE_OR_DIE(fourCC_S_2 == FOURCC_S_2, "File format doesn't match: 3");
		unsigned char fourCC_S_3 = headerParser.ParseByte();
		GUARANTEE_OR_DIE(fourCC_S_3 == FOURCC_S_3, "File format doesn't match: 4");

		unsigned char cohort_id = headerParser.ParseByte();
		GUARANTEE_OR_DIE(cohort_id == COHORT_ID, "Cohort ID doesn't match");
		unsigned char major_file_v = headerParser.ParseByte();
		GUARANTEE_OR_DIE(major_file_v == MAJOR_FILE_V, "Major file version doesn't match");
		unsigned char minor_file_v = headerParser.ParseByte();
		GUARANTEE_OR_DIE(minor_file_v == MINOR_FILE_V, "Minor file version doesn't match");
		
		unsigned char endianness = headerParser.ParseByte();
		headerParser.SetEndianMode((EndianMode)endianness);

		uint32_t locationOfTOC = headerParser.ParseUint32();
		uint32_t sizeOfTOC = (uint32_t)buffer.size() - locationOfTOC;

		BufferParser tocParser = BufferParser(buffer.data() + locationOfTOC, sizeOfTOC);
		unsigned char toc_fourCC_S_0 = tocParser.ParseByte();
		GUARANTEE_OR_DIE(toc_fourCC_S_0 == TOC_FOURCC_S_0, "TOC format doesn't match: 1");
		unsigned char toc_fourCC_S_1 = tocParser.ParseByte();
		GUARANTEE_OR_DIE(toc_fourCC_S_1 == TOC_FOURCC_S_1, "TOC format doesn't match: 2");
		unsigned char toc_fourCC_S_2 = tocParser.ParseByte();
		GUARANTEE_OR_DIE(toc_fourCC_S_2 == TOC_FOURCC_S_2, "TOC format doesn't match: 3");
		unsigned char toc_fourCC_S_3 = tocParser.ParseByte();
		GUARANTEE_OR_DIE(toc_fourCC_S_3 == TOC_FOURCC_S_3, "TOC format doesn't match: 4");

		uint8_t numOfChunk = tocParser.ParseByte();
		tocParser.SetEndianMode((EndianMode)endianness);
		for (uint8_t i = 0; i < numOfChunk; i++) {
			unsigned char chunkType = tocParser.ParseByte();
			uint32_t start = tocParser.ParseUint32();
			uint32_t size = tocParser.ParseUint32();
			BufferParser bufParser = BufferParser(buffer.data() + start, size);
			ParseChunkByType((ChunkType)chunkType, bufParser);
		}
	}
	else {
		ERROR_AND_DIE(Stringf("File: %s doesn't exist", file.c_str()));
	}
	return true;
}



GameBasic2D::GameBasic2D() : Game()
{
	m_clock = new Clock(Clock::GetSystemClock());
	AABB2 viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.m_viewport = viewport;
	m_worldCamera.m_viewport = viewport;
	m_status = GameStatus::BASIC_2D;

	Image nbg = Image("Data/Images/TestUV.png");
	m_natureBackground = g_theRenderer->CreateTextureFromImage(nbg);
	
}

GameBasic2D::~GameBasic2D()
{
	m_natureBackground = nullptr;
}

void GameBasic2D::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	m_player = new Player(this, Vec3(0.f, 0.f, 0.f));
	m_player->m_camera.m_viewport = m_screenCamera.m_viewport;
	m_prop1 = new Prop(this, Vec3(2.f, 2.f, 0.0f));
	//AddVertsForAABB3D(m_prop1->m_vertexes, AABB3(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)));
	//m_prop1->m_texture = g_textureLibrary[TEXTURE_CUBE];
	float maxX = 0.5f;
	float maxY = 0.5f;
	float maxZ = 0.5f;
	float minX = -0.5f;
	float minY = -0.5f;
	float minZ = -0.5f;

	g_theEventSystem->SubscribeEventCallbackFunction("SaveCurrentScene", SaveSceneToFile);
	g_theEventSystem->SubscribeEventCallbackFunction("LoadScene", LoadSceneFromFile);

	// +x
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_prop2 = new Prop(this, Vec3(-2.f, -2.f, 0.f));
	
	// +x
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_sphere = new Prop(this, Vec3(10.f, -5.f, 1.f));
	AddVertsForSphere3D(m_sphere->m_vertexes, Vec3(0.f, 0.f, 0.f), 1.f);
	//AddVertsForCylinder3D(m_sphere->m_vertexes, Vec3(1.f, 1.f, 1.f), Vec3(2.f, 2.f, 2.f), 0.5f);
	//AddVertsForCone3D(m_sphere->m_vertexes, Vec3(-1.f, -1.f, -1.f), Vec3(-2.f, -2.f, -2.f), 0.5f);
	//AddVertsForQuad3D(m_sphere->m_vertexes, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 1.f));
	m_sphere->m_texture = g_textureLibrary[TEXTURE_SPHERE];

	//for (int objectIndex = 0; objectIndex < m_; objectIndex++) {
	//	ConvexSceneObject cso = ConvexSceneObject();
	//	m_objects.push_back(cso);
	//}
	m_objects.resize(m_numOfObjects);
	CreateDiscTree();
	
	m_arrowStart = m_worldBound.GetDimensions() * 0.2f + m_worldBound.m_mins;
	m_arrowEnd = m_worldBound.GetDimensions() * 0.7f + m_worldBound.m_mins;

	SetCamera();

}

void GameBasic2D::Update()
{
	if (g_theGameStatus != m_status) {
		Exit();
		g_theGames[(int)g_theGameStatus]->Enter();
	}
	UpdateFromKeyboard();
	UpdateFromController();

	SetCamera();

	
	if (!m_isPaused)
	{
		UpdateEntities();

		DeleteGarbageEntities();
	}
	
}


void GameBasic2D::Render() const
{
	if (m_isAttractMode)
	{
		RenderAttractMode();
		return;
	}
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	
	RenderEntities();

	
	RenderUI();

	if (m_isPaused)
	{
		RenderPaused();
	}
	
	DebugRenderWorld(m_player->m_camera);
	DebugRenderScreen(m_screenCamera);
}

void GameBasic2D::Shutdown()
{
	
}

void GameBasic2D::Enter()
{

}

void GameBasic2D::Exit()
{

}

void GameBasic2D::UpdateEntities()
{
	float dist = (m_arrowEnd - m_arrowStart).GetLength();
	Vec2 fwdNormal;

	if (m_numOfObjects > (int)m_objects.size())
	{
		m_objects.resize(m_numOfObjects);
		m_isTreeUpdated = false;
	}
	if (!m_isTreeUpdated && !m_isDraggingObj)
	{
		CreateDiscTree();
		m_isTreeUpdated = true;
	}
	if (!m_isDraggingObj)
	{
		bool foundCurrent = false;
		for (int objectIndex = 0; objectIndex < m_numOfObjects; objectIndex++) {
			if (m_objects[objectIndex].m_hull.IsPointInside(m_cursor) && !foundCurrent) {
				m_focusedPolyIndex = objectIndex;
				foundCurrent = true;
				break;
			}
		}
		if (!foundCurrent)
			m_focusedPolyIndex = -1;
	}

	if (dist != 0.f)
		fwdNormal = (m_arrowEnd - m_arrowStart) / dist;
	else fwdNormal = Vec2();

	m_impactPos = m_cursor;
	m_impactNormal = Vec2();
	m_didImpact = false;

	m_previousCursor = m_cursor;
	m_cursor = g_theWindow->GetNormalizedCursorPos();
	m_cursor.x *= m_worldBound.GetDimensions().x;
	m_cursor.y *= m_worldBound.GetDimensions().y;
	m_cursor += m_worldBound.m_mins;

	//if (!m_setStart)
		//m_arrowStart = m_cursor;
	//if (!m_setEnd)
		//m_arrowEnd = m_cursor;


	if (m_isDraggingObj)
	{
		Vec2 displacement = m_cursor - m_previousCursor;
		m_objects[m_focusedPolyIndex].Translate(displacement);
		m_isTreeUpdated = false;
	}

	RaycastResult2D res = RaycastVsObjects(m_arrowStart, fwdNormal, dist);
	m_didImpact = res.m_didImpact;
	m_impactPos = res.m_impactPos;
	m_impactNormal = res.m_impactNormal;

	

	if (m_isRaytesting) 
	{
		for (int i = 0; i < m_numOfRay - 1; i++)
		{
			Vec2 start;
			start.x = g_rng->RollRandomFloatInRange(m_worldBound.m_mins.x, m_worldBound.m_maxs.x);
			start.y = g_rng->RollRandomFloatInRange(m_worldBound.m_mins.y, m_worldBound.m_maxs.y);
			float radius = g_rng->RollRandomFloatInRange(0.f, 360.f);
			Vec2 normal = Vec2::MakeFromPolarDegrees(radius);
			float maxDist = g_rng->RollRandomFloatInRange(5.f, 100.f);
			RaycastVsObjects(start, normal, maxDist);
		}
	}

	m_player->Update();
}

void GameBasic2D::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");

	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");

}

void GameBasic2D::DeleteGarbageEntities()
{

}

void GameBasic2D::KillAll()
{

}

void GameBasic2D::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->WasKeyJustPressed('R'))
	{
		m_isRaytesting = !m_isRaytesting;
	}
	if (g_theInput->WasKeyJustPressed(' ') || g_theInput->WasKeyJustPressed('N'))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
			
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_isPaused = true;
		SetCamera();

		if (m_isAttractMode)
		{
			return;
		}

		UpdateEntities();

		DeleteGarbageEntities();

		if (m_isShaking)
		{
			float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
			float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
			m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
			m_shakingTime += m_clock->GetDeltaSeconds();
			if (m_shakingTime >= 1.f)
			{
				m_shakingTime = 0.f;
				m_isShaking = false;
			}
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebugBoundingDisc = !m_isDebugBoundingDisc;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F2))
	{
		m_isTransparent = !m_isTransparent;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F4))
	{
		m_isDebugPS = !m_isDebugPS;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F9))
	{
		m_isOptimized = !m_isOptimized;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		for (int i = 0; i < m_numOfObjects; i++) {
			m_objects[i].Randomize();
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		/*
		m_arrowStart = g_theWindow->GetNormalizedCursorPos();
		m_arrowStart.x *= m_worldBound.GetDimensions().x;
		m_arrowStart.y *= m_worldBound.GetDimensions().y;
		m_arrowStart += m_worldBound.m_mins;
		m_setStart = !m_setStart;*/
		if (m_focusedPolyIndex >= 0 && !m_isDraggingObj)
		{
			m_isDraggingObj = true;
		}
		else if (m_isDraggingObj)
			m_isDraggingObj = false;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE))
	{
		m_arrowEnd = g_theWindow->GetNormalizedCursorPos();
		m_arrowEnd.x *= m_worldBound.GetDimensions().x + m_worldBound.m_mins.x;
		m_arrowEnd.y *= m_worldBound.GetDimensions().y + m_worldBound.m_mins.y;
		m_arrowEnd += m_worldBound.m_mins;
		m_setEnd = !m_setEnd;
	}

	float deltaSeconds = m_clock->GetDeltaSeconds();

	if (g_theInput->IsKeyDown(KEYCODE_UPARROW))
	{
		m_arrowStart.y += deltaSeconds * 20.f;
		m_arrowEnd.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW))
	{
		m_arrowStart.y -= deltaSeconds * 20.f;
		m_arrowEnd.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW))
	{
		m_arrowStart.x -= deltaSeconds * 20.f;
		m_arrowEnd.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW))
	{
		m_arrowStart.x += deltaSeconds * 20.f;
		m_arrowEnd.x += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('Q'))
	{
		//m_arrowStart.y += deltaSeconds * 20.f;
		if (m_focusedPolyIndex >= 0 && !m_isDraggingObj) {
			m_objects[m_focusedPolyIndex].RotateAboutPoint(m_cursor, m_clock->GetDeltaSeconds() * 30.f);
			m_isTreeUpdated = false;
		}
			
	}
	if (g_theInput->IsKeyDown('E'))
	{
		//m_arrowStart.y += deltaSeconds * 20.f;
		if (m_focusedPolyIndex >= 0 && !m_isDraggingObj){
			m_objects[m_focusedPolyIndex].RotateAboutPoint(m_cursor, -m_clock->GetDeltaSeconds() * 30.f);
			m_isTreeUpdated = false;
		}
		else m_arrowEnd = m_cursor;
	}
	if (g_theInput->IsKeyDown('S'))
	{
		m_arrowStart = m_cursor;
	}
	if (g_theInput->IsKeyDown('L'))
	{
		if (m_focusedPolyIndex >= 0 && !m_isDraggingObj)
		{
			m_objects[m_focusedPolyIndex].ScaleAboutPoint(m_cursor, 1.f - m_clock->GetDeltaSeconds() <= 0.1f ? 0.1f : 1.f - m_clock->GetDeltaSeconds());
			m_isTreeUpdated = false;
		}
			
	}
	if (g_theInput->IsKeyDown('K'))
	{
		if (m_focusedPolyIndex >= 0 && !m_isDraggingObj)
		{
			m_objects[m_focusedPolyIndex].ScaleAboutPoint(m_cursor, 1.f + m_clock->GetDeltaSeconds() >= 3.f ? 3.f : 1.f + m_clock->GetDeltaSeconds());
			m_isTreeUpdated = false;
		}
			
	}
	if (g_theInput)

	if (g_theInput->WasKeyJustPressed(KEYCODE_COMMA))
	{
		if (m_numOfObjects != 1)
		{
			m_numOfObjects = m_numOfObjects >> 1;
			m_objects.clear();
			m_objects.resize(m_numOfObjects);
			m_isTreeUpdated = false;
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_PERIOD))
	{
		m_numOfObjects = m_numOfObjects << 1; 
	}

	if (g_theInput->WasKeyJustPressed('N'))
	{
		if (m_numOfRay != 0)
		{
			m_numOfRay = m_numOfRay >> 1;
		}
	}
	if (g_theInput->WasKeyJustPressed('M'))
	{
		m_numOfRay = m_numOfRay << 1;
		//m_numOfObjects = m_numOfObjects << 1;
	}

	if (g_theInput->WasKeyJustPressed('V'))
	{
		/*
		std::vector<uint8_t> buffer;
		buffer.reserve(1000);
		BufferWriter bufwriter = BufferWriter(m_buffer);
		AppendTestFileBufferData(bufwriter, EndianMode::BIG);
		WriteToFile(m_buffer, "Data/Test/test.testbig");*/
		EventArgs args;
		args.SetValue("file", "Data/Test/test01.GHCS");
		SaveSceneToFile(args);
	}
	if (g_theInput->WasKeyJustPressed('B'))
	{
		/*
		std::vector<uint8_t> buffer;
		FileReadToBuffer(buffer, "Data/Test/test.testbig");
		BufferParser bufParser = BufferParser(buffer.data(), buffer.size());
		ParseTestFileBufferData(bufParser, EndianMode::BIG);*/
		EventArgs args;
		args.SetValue("file", "Data/Test/test01.GHCS");
		LoadSceneFromFile(args);
	}
	if (g_theInput->WasKeyJustPressed('C'))
	{
		/*
		std::vector<uint8_t> buffer;
		FileReadToBuffer(buffer, "Data/Test/test.testbig");
		BufferParser bufParser = BufferParser(buffer.data(), buffer.size());
		ParseTestFileBufferData(bufParser, EndianMode::BIG);*/
		EventArgs args;
		args.SetValue("file", "Data/Test/pikachu.GHCS");
		LoadSceneFromFile(args);
	}
}

void GameBasic2D::UpdateFromController()
{
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
	}
}

RaycastResult2D GameBasic2D::RaycastVsObjects(Vec2 start, Vec2 fwdNormal, float maxDist) const
{
	RaycastResult2D res;
	if (m_isOptimized && m_isTreeUpdated) {
		std::queue<Disc2TreeNode> discQueue;
		discQueue.push(m_disc2Tree[(int)m_disc2Tree.size() - 1]);
		float minDist = FLT_MAX;
		while (!discQueue.empty())
		{
			Disc2TreeNode parent = discQueue.front();
			discQueue.pop();
			if (parent.m_leftChild != -1)
			{
				Disc2TreeNode left = m_disc2Tree[parent.m_leftChild];
				RaycastResult2D temp = RaycastVsDisc2D(start, fwdNormal, maxDist, left.m_discCenter, left.m_discRadius);
				if (temp.m_didImpact)
				{
					discQueue.push(left);
				}
			}
			if (parent.m_rightChild != -1) {
				Disc2TreeNode right = m_disc2Tree[parent.m_rightChild];
				RaycastResult2D temp = RaycastVsDisc2D(start, fwdNormal, maxDist, right.m_discCenter, right.m_discRadius);
				if (temp.m_didImpact)
				{
					discQueue.push(right);
				}
			}
			if (parent.m_leftChild == -1 && parent.m_rightChild == -1)
			{
				for (int i = 0; i < (int)parent.m_entityIndex.size(); i++)
				{
					int index = parent.m_entityIndex[i];
					RaycastResult2D temp = m_objects[index].m_hull.RaycastVsConvexHull2D(start, fwdNormal, maxDist);
					if (temp.m_didImpact && temp.m_impactDis < minDist) {
						res = temp;
						minDist = temp.m_impactDis;
					}
				}
			}
		}
	}
	else {
		float minDist = FLT_MAX;
		for (int i = 0; i < m_numOfObjects; i++) {
			RaycastResult2D discRes = RaycastVsDisc2D(start, fwdNormal, maxDist, m_objects[i].m_boundingDiscCenter, m_objects[i].m_boundingDiscRadius);
			if (discRes.m_didImpact) {
				RaycastResult2D temp = m_objects[i].m_hull.RaycastVsConvexHull2D(start, fwdNormal, maxDist);
				if (temp.m_didImpact && temp.m_impactDis < minDist) {
					res = temp;
					minDist = temp.m_impactDis;
				}
			}
		}
	}
	return res;
}

void GameBasic2D::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(100, 255, 255));
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
			(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameBasic2D::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Raycast Vs Convex (2D)";
	std::string controls = "F8 to randomize; LMB/RMB set ray start/end; ESDF move start, IJKL move end, hold T = slow";
	std::string details = Stringf("Object: %d; Ray: %d; FPS: %.2f; Partitioning: Disc2 Tree ", m_numOfObjects, m_numOfRay, 1.f / m_clock->GetDeltaSeconds());
	if (m_isOptimized) {
		details += "On; ";
	}
	else details += "Off; ";
	if (m_isRaytesting) {
		details += "Invisible Raycast Test Enabled";
	}
	else details += "Invisible Raycast Test Disabled";
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 90.f * 8.f, 200.f * 8.f, 100.f * 8.f), 2.5f * 8.f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 77.f * 8.f, 200.f * 8.f, 100.f * 8.f), 2.2f * 8.f, details.c_str(), Rgba8(200, 0, 200), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->EndCamera(m_screenCamera);
}


void GameBasic2D::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8::WHITE);

	g_theRenderer->SetModelConstants();

	if (m_isTransparent) {
		//bool foundCurrent = false;
		for (int objectIndex = 0; objectIndex < m_numOfObjects; objectIndex++) {
			/*
			if (m_objects[objectIndex].m_hull.IsPointInside(m_cursor) && !foundCurrent) {
				m_objects[objectIndex].RenderHighlight();
				foundCurrent = true;
			}
			else m_objects[objectIndex].Render();*/
			if (objectIndex == m_focusedPolyIndex)
				m_objects[objectIndex].RenderHighlight();
			else m_objects[objectIndex].Render();
		}
	}
	else {
		for (int objectIndex = 0; objectIndex < m_numOfObjects; objectIndex++) {
			m_objects[objectIndex].RenderFrame();
		}
		//bool foundCurrent = false;
		for (int objectIndex = 0; objectIndex < m_numOfObjects; objectIndex++) {
			/*
			if (m_objects[objectIndex].m_hull.IsPointInside(m_cursor) && !foundCurrent) {
				m_objects[objectIndex].RenderFilledHighlight();
				foundCurrent = true;
			}
			else m_objects[objectIndex].RenderFilled();*/
			if (objectIndex == m_focusedPolyIndex)
				m_objects[objectIndex].RenderFilledHighlight();
			else m_objects[objectIndex].RenderFilled();
		}
	}

	if (m_numOfObjects == 1) {
		ConvexSceneObject obj = m_objects[0];
		std::vector<Vec2> points = obj.m_poly.GetAllPoints();
		std::vector<Vertex_PCU> lineVerts;
		for (int i = 0; i < (int)points.size(); i++) {
			Vec2 end;
			if (i == (int)points.size() - 1)
				end = points[0];
			else end = points[i + 1];
			Vec2 normal = (m_arrowEnd - m_arrowStart).GetNormalized();
			float length = (m_arrowEnd - m_arrowStart).GetLength();
			RaycastResult2D res = obj.m_hull.m_planes[i].RaycastVsPlane2(m_arrowStart, normal, length);
			if (!res.m_didImpact)
				AddVertsForInfiniteLineSegment2D(lineVerts, points[i], end, 0.5f, Rgba8::RED);
			else {
				AddVertsForInfiniteLineSegment2D(lineVerts, points[i], end, 0.5f, Rgba8::GREEN);
				AddVertsForDisc2D(lineVerts, res.m_impactPos, 0.8f, Rgba8::BLUE);
			}
		}
		g_theRenderer->DrawVertexArray((int)lineVerts.size(), lineVerts.data());
	}

	std::vector<Vertex_PCU> arrowVerts;
	if (!m_didImpact) {
		AddVertsForArrow2D(arrowVerts, m_arrowStart, m_arrowEnd, 1.f, 0.2f, Rgba8(0, 255, 0));
		g_theRenderer->DrawVertexArray((int)arrowVerts.size(), arrowVerts.data());
	}
	else {
		AddVertsForArrow2D(arrowVerts, m_arrowStart, m_arrowEnd, 1.f, 0.2f, Rgba8(127, 127, 127));
		g_theRenderer->DrawVertexArray((int)arrowVerts.size(), arrowVerts.data());
		std::vector<Vertex_PCU> impactVerts;
		AddVertsForArrow2D(impactVerts, m_arrowStart, m_impactPos, 1.f, 0.2f, Rgba8(255, 0, 0));
		g_theRenderer->DrawVertexArray((int)impactVerts.size(), impactVerts.data());
		std::vector<Vertex_PCU> impactNormalVerts;
		AddVertsForArrow2D(impactNormalVerts, m_impactPos, m_impactPos + m_impactNormal * 10.f, 1.f, 0.2f, Rgba8(200, 200, 0));
		g_theRenderer->DrawVertexArray((int)impactNormalVerts.size(), impactNormalVerts.data());
		std::vector<Vertex_PCU> impactPosVerts;
		AddVertsForDisc2D(impactPosVerts, m_impactPos, 0.5f, Rgba8(255, 255, 255));
		g_theRenderer->DrawVertexArray((int)impactPosVerts.size(), impactPosVerts.data());
	}

	DrawCircle(m_cursor, 0.5f, Rgba8::BLUE);

	if (m_isDebugBoundingDisc)
	{
		//DebugRenderEntities();
		for (int i = 0; i < m_numOfObjects; i++)
		{
			m_objects[i].DebugRender();
		}
	}

	if (m_isDebugPS)
	{
		for (int i = 0; i < (int)m_disc2Tree.size(); i++)
		{
			m_disc2Tree[i].DebugRender();
		}
	}

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameBasic2D::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameBasic2D::RenderGrid() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);
	//g_theRenderer->SetModelConstants();
	//std::vector<Vertex_PCU> sphere;
	//AddVertsForSpere3D(sphere, Vec3(10.f, -5.f, 1.f), 1.f);
	//g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_SPHERE]);
	//g_theRenderer->DrawVertexArray((int)sphere.size(), sphere.data());
	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(nullptr);
	std::vector<Vertex_PCU> grids;
	for (int xIndex = -50; xIndex <= 50; xIndex++) {
		float offset = 0.05f;
		Rgba8 color = Rgba8::GREY;
		if (xIndex % 5 == 0) {
			color = Rgba8::GREEN;
			offset *= 2;
		}
		Vec2 mins = Vec2(float(xIndex) - offset, -50.f);
		Vec2 maxs = Vec2(float(xIndex) + offset, 50.f);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}

	for (int yIndex = -50; yIndex <= 50; yIndex++) {
		float offset = 0.05f;
		Rgba8 color = Rgba8::GREY;
		if (yIndex % 5 == 0) {
			color = Rgba8::RED;
			offset *= 2;
		}
		Vec2 mins = Vec2(-50.f, float(yIndex) - offset);
		Vec2 maxs = Vec2(50.f, float(yIndex) + offset);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}
	g_theRenderer->DrawVertexArray((int)grids.size(), grids.data());
	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameBasic2D::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameBasic2D::EnterGame()
{
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	//PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameBasic2D::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_player->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_worldCamera.SetOrthoView(m_worldBound.m_mins, m_worldBound.m_maxs);
		//Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
}

void GameBasic2D::CreateDiscTree()
{
	m_disc2Tree.clear();
	std::sort(m_objects.begin(), m_objects.end());
	std::queue<Disc2TreeNode> discQueue;
	for (int i = 0; i < m_numOfObjects; i++)
	{
		Disc2TreeNode node;
		node.m_entityIndex.push_back(i);
		node.m_discCenter = m_objects[i].m_boundingDiscCenter;
		node.m_discRadius = m_objects[i].m_boundingDiscRadius;
		discQueue.push(node);
	}
	int index = 0;
	while (discQueue.size() > 1)
	{
		Disc2TreeNode parentNode;

		Disc2TreeNode leftNode = discQueue.front();
		m_disc2Tree.push_back(leftNode);
		parentNode.m_leftChild = index++;
		discQueue.pop();
		Disc2TreeNode rightNode = discQueue.front();
		m_disc2Tree.push_back(rightNode);
		parentNode.m_rightChild = index++;
		discQueue.pop();

		Vec2 LToRNormal = (rightNode.m_discCenter - leftNode.m_discCenter).GetNormalized();
		float length = (rightNode.m_discCenter - leftNode.m_discCenter).GetLength();
		parentNode.m_discRadius = (leftNode.m_discRadius + length + rightNode.m_discRadius) * 0.5f;
		parentNode.m_discCenter = rightNode.m_discCenter + (rightNode.m_discRadius - parentNode.m_discRadius) * LToRNormal;

		discQueue.push(parentNode);
	}
	Disc2TreeNode root = discQueue.front();
	m_disc2Tree.push_back(root);
}
