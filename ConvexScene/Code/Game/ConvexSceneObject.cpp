#include "Game/ConvexSceneObject.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"

ConvexSceneObject::ConvexSceneObject()
{
	Randomize();
}

ConvexSceneObject::ConvexSceneObject(BufferParser& bufferParse)
{
	m_poly = ConvexPoly2D(bufferParse);
	m_hull = ConvexHull2D(m_poly);
	m_boundingDiscCenter = bufferParse.ParseVec2();
	m_boundingDiscRadius = bufferParse.ParseFloat();
}

ConvexSceneObject::~ConvexSceneObject()
{
}

void ConvexSceneObject::Update()
{

}

void ConvexSceneObject::Render() const
{
	std::vector<Vertex_PCU> verts;
	std::vector<unsigned int> indexes;

	AddVertsForConvexPoly2DFrame(verts, indexes, m_poly, 0.5f, Rgba8::BLACK);
	AddVertsForConvexPoly2D(verts, indexes, m_poly, Rgba8(0, 200, 200, 127));

	g_theRenderer->DrawVertexIndexArray((int)verts.size(), (int)indexes.size(), verts.data(), indexes.data());
}

void ConvexSceneObject::RenderHighlight() const
{
	std::vector<Vertex_PCU> verts;
	std::vector<unsigned int> indexes;

	AddVertsForConvexPoly2DFrame(verts, indexes, m_poly, 0.5f, Rgba8::BLACK);
	AddVertsForConvexPoly2D(verts, indexes, m_poly, Rgba8(0, 255, 255, 127));

	g_theRenderer->DrawVertexIndexArray((int)verts.size(), (int)indexes.size(), verts.data(), indexes.data());
}

void ConvexSceneObject::RenderFrame() const
{
	std::vector<Vertex_PCU> verts;
	std::vector<unsigned int> indexes;

	AddVertsForConvexPoly2DFrame(verts, indexes, m_poly, 0.5f, Rgba8::BLACK);

	g_theRenderer->DrawVertexIndexArray((int)verts.size(), (int)indexes.size(), verts.data(), indexes.data());
}

void ConvexSceneObject::RenderFilled() const
{
	std::vector<Vertex_PCU> verts;
	std::vector<unsigned int> indexes;

	AddVertsForConvexPoly2D(verts, indexes, m_poly, Rgba8(0, 200, 200, 255));

	g_theRenderer->DrawVertexIndexArray((int)verts.size(), (int)indexes.size(), verts.data(), indexes.data());
}

void ConvexSceneObject::RenderFilledHighlight() const
{
	std::vector<Vertex_PCU> verts;
	std::vector<unsigned int> indexes;

	AddVertsForConvexPoly2D(verts, indexes, m_poly, Rgba8(0, 255, 255, 255));

	g_theRenderer->DrawVertexIndexArray((int)verts.size(), (int)indexes.size(), verts.data(), indexes.data());
}

void ConvexSceneObject::DebugRender() const
{
	DebugDrawRing(m_boundingDiscCenter, m_boundingDiscRadius, 0.4f, Rgba8::RED);
}

void ConvexSceneObject::Randomize()
{
	float minRadius = 2.f;
	float maxRadius = 15.f;

	float minX = 5.f;
	float maxX = 195.f;

	float minY = 5.f;
	float maxY = 95.f;

	m_boundingDiscRadius = g_rng->RollRandomFloatInRange(minRadius, maxRadius);
	float centerX = g_rng->RollRandomFloatInRange(minX, maxX);
	float centerY = g_rng->RollRandomFloatInRange(minY, maxY);

	m_boundingDiscCenter = Vec2(centerX, centerY);

	float minStart = 0.f;
	float midStart = 30.f;
	float maxStart = 90.f;

	float sum = g_rng->RollRandomFloatInRange(minStart, maxStart);
	std::vector<Vec2> points;
	while (sum < 360.f) {
		points.push_back(Vec2::MakeFromPolarDegrees(sum, m_boundingDiscRadius) + m_boundingDiscCenter);
		sum += g_rng->RollRandomFloatInRange(midStart, maxStart);
		
	}
	m_poly = ConvexPoly2D(points);
	m_hull = ConvexHull2D(m_poly);
}

void ConvexSceneObject::WritePolyIntoBuffer(BufferWriter& bufWriter)
{
	m_poly.WriteToBuffer(bufWriter);
}


void ConvexSceneObject::WriteHullIntoBuffer(BufferWriter& bufWriter)
{
	m_hull.WriteToBuffer(bufWriter);
}

void ConvexSceneObject::WriteDiscIntoBuffer(BufferWriter& bufWriter)
{
	bufWriter.AppendVec2(m_boundingDiscCenter);
	bufWriter.AppendFloat(m_boundingDiscRadius);
}

void ConvexSceneObject::ParsePolyFromBuffer(BufferParser& bufParser)
{
	m_poly.ParseFromBuffer(bufParser);
	m_hull = ConvexHull2D(m_poly);
}

void ConvexSceneObject::ParseHullFromBuffer(BufferParser& bufParser)
{
	m_hull.ParseFromBuffer(bufParser);
}

void ConvexSceneObject::ParseDiscFromBuffer(BufferParser& bufParser)
{
	m_boundingDiscCenter = bufParser.ParseVec2();
	m_boundingDiscRadius = bufParser.ParseFloat();
}

void ConvexSceneObject::RotateAboutPoint(Vec2 point, float degree)
{
	m_poly.RotateAboutPoint(point, degree);
	m_hull = ConvexHull2D(m_poly);
	m_boundingDiscCenter.RotateAboutPoint(point, degree);
}

void ConvexSceneObject::ScaleAboutPoint(Vec2 point, float scale)
{
	m_poly.ScaleAboutPoint(point, scale);
	m_hull = ConvexHull2D(m_poly);
	m_boundingDiscRadius *= scale;
	m_boundingDiscCenter.ScaleAboutPoint(point, scale);
}

void ConvexSceneObject::Translate(Vec2 translation)
{
	m_poly.Translate(translation);
	m_hull = ConvexHull2D(m_poly);
	m_boundingDiscCenter += translation;
}

AABB2 ConvexSceneObject::GetBoundingAABB2() const
{
	AABB2 boundingAABB;
	boundingAABB.SetCenter(m_boundingDiscCenter);
	boundingAABB.SetDimensions(Vec2(m_boundingDiscRadius, m_boundingDiscRadius) * 2.f);
	return boundingAABB;
}

bool ConvexSceneObject::operator<(const ConvexSceneObject& compare) const
{
	float leftA = m_boundingDiscCenter.x;
		//- m_boundingDiscRadius;
	float leftB = compare.m_boundingDiscCenter.x;
		//- compare.m_boundingDiscRadius;

	float bottomA = m_boundingDiscCenter.y;
		//- m_boundingDiscRadius;
	float bottomB = compare.m_boundingDiscCenter.y;
		//- compare.m_boundingDiscRadius;
	if (leftA < leftB)
		return true;
	else if (leftA == leftB)
	{
		if (bottomA < bottomB)
			return true;
	}
	return false;
}

Disc2TreeNode::Disc2TreeNode()
{

}

Disc2TreeNode::~Disc2TreeNode()
{

}

void Disc2TreeNode::DebugRender() const
{
	DebugDrawRing(m_discCenter, m_discRadius, 0.3f, Rgba8::YELLOW);
}

void Disc2TreeNode::WriteIntoBuffer(BufferWriter& bufWriter)
{
	bufWriter.AppendVec2(m_discCenter);
	bufWriter.AppendFloat(m_discRadius);
	uint32_t numOfEntity = (uint32_t)m_entityIndex.size();
	bufWriter.AppendUint32(numOfEntity);
	for (unsigned int j = 0; j < numOfEntity; j++) {
		bufWriter.AppendInt32(m_entityIndex[j]);
	}
	bufWriter.AppendInt32(m_leftChild);
	bufWriter.AppendInt32(m_rightChild);
}

void Disc2TreeNode::ParseFromBuffer(BufferParser& bufParser)
{
	m_discCenter = bufParser.ParseVec2();
	m_discRadius = bufParser.ParseFloat();
	uint32_t numOfEntity = bufParser.ParseUint32();
	for (unsigned int i = 0; i < numOfEntity; i++) {
		m_entityIndex.push_back(bufParser.ParseInt32());
	}
	m_leftChild = bufParser.ParseInt32();
	m_rightChild = bufParser.ParseInt32();
}

