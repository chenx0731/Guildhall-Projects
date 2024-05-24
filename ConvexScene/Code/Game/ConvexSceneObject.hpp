#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/ConvexPoly2D.hpp"
#include "Engine/Math/ConvexHull2D.hpp"

class BufferParser;
class BufferWriter;


class ConvexSceneObject
{
public:
	ConvexSceneObject();
	ConvexSceneObject(BufferParser& bufferParse);
	~ConvexSceneObject();

	void Update();
	void Render() const;
	void RenderHighlight() const;
	void RenderFrame() const;
	void RenderFilled() const;
	void RenderFilledHighlight() const;
	void DebugRender() const;

	void Randomize();

	void WritePolyIntoBuffer(BufferWriter& bufWriter);
	void WriteHullIntoBuffer(BufferWriter& bufWriter);
	void WriteDiscIntoBuffer(BufferWriter& bufWriter);

	void ParsePolyFromBuffer(BufferParser& bufParser);
	void ParseHullFromBuffer(BufferParser& bufParser);
	void ParseDiscFromBuffer(BufferParser& bufParser);

	void RotateAboutPoint(Vec2 point, float degree);

	void ScaleAboutPoint(Vec2 point, float scale);

	void Translate(Vec2 translation);

	AABB2 GetBoundingAABB2() const;

	bool		operator<(const ConvexSceneObject& compare) const;
public:
	ConvexPoly2D	m_poly;
	ConvexHull2D	m_hull;
	Vec2			m_boundingDiscCenter;
	float			m_boundingDiscRadius;
};

class Disc2TreeNode
{
public:
	Disc2TreeNode();
	~Disc2TreeNode();
	void DebugRender() const;
	void WriteIntoBuffer(BufferWriter& bufWriter);
	void ParseFromBuffer(BufferParser& bufParser);
public:
	Vec2				m_discCenter;
	float				m_discRadius = 0.f;
	std::vector<int>	m_entityIndex;
	int					m_leftChild = -1;
	int					m_rightChild = -1;
};