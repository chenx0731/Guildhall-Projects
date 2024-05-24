#pragma once
#include "Game/BlockIterator.hpp"
#include "Engine/Math/RaycastResult2D.hpp"

struct GameRaycastResult : RaycastResult3D
{
public:
	BlockIterator	m_impactBlock;
	FaceDirt		m_impactFace;
};