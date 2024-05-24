#define UNUSED(x) (void)(x);
#include "Engine/Renderer/Voxelization.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB3.hpp"

void VoxelizeModel(const std::vector<Vertex_PCUTBN>& vertexes, const std::vector<unsigned int>& indexes, std::vector<Vec3>& centers)
{
	UNUSED(centers);
	UNUSED(indexes);
	float maxF = FLT_MAX;
	Vec3 maxV = Vec3(maxF, maxF, maxF);
	AABB3 box = AABB3(maxV, -maxV);
	for (int vertIndex = 0; vertIndex < (int)vertexes.size(); vertIndex++) {
		if (vertexes[vertIndex].m_position.x > box.m_maxs.x)
			box.m_maxs.x = vertexes[vertIndex].m_position.x;
		if (vertexes[vertIndex].m_position.y > box.m_maxs.y)
			box.m_maxs.y = vertexes[vertIndex].m_position.y;
		if (vertexes[vertIndex].m_position.z > box.m_maxs.z)
			box.m_maxs.z = vertexes[vertIndex].m_position.z;

		if (vertexes[vertIndex].m_position.x < box.m_mins.x)
			box.m_mins.x = vertexes[vertIndex].m_position.x;
		if (vertexes[vertIndex].m_position.y < box.m_mins.y)
			box.m_mins.y = vertexes[vertIndex].m_position.y;
		if (vertexes[vertIndex].m_position.z < box.m_mins.z)
			box.m_mins.z = vertexes[vertIndex].m_position.z;
	}

	Camera cameraX;
	Camera cameraY;
	Camera cameraZ;

	cameraX.m_position = box.GetCenter();
	cameraX.m_position.x = box.m_mins.x - 0.2f;
	
	cameraX.SetOrthoView(Vec2(box.m_mins.y - 0.1f, box.m_mins.z - 0.1f), Vec2(box.m_maxs.y + 0.1f, box.m_maxs.z + 0.1f));

	TextureConfig countConfig;
	
	//countConfig.m
}
