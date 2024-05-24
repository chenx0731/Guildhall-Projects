#pragma once
#include "Engine/Core/Job.hpp"
#include <vector>

class Chunk;
class ChunkGenerateJob : public Job
{
public:
	ChunkGenerateJob(Chunk* chunk);
	virtual void Execute() override;

public:
	Chunk* m_chunk;
};


	