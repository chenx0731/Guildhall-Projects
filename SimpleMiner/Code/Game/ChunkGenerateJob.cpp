#include "Game/ChunkGenerateJob.hpp"
#include "Game/Chunk.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <thread>

ChunkGenerateJob::ChunkGenerateJob(Chunk* chunk)
{
	m_chunk = chunk;
}

void ChunkGenerateJob::Execute()
{
	if (m_chunk) {
		m_chunk->m_chunkState = ChunkState::ACTIVATING_GENERATING;
		m_chunk->PopulateBlocks();
		m_chunk->m_chunkState = ChunkState::ACTIVATING_GENERATE_COMPLETE;
	}
}
