#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Mat44.hpp"
class Texture;
struct Vertex_PCU;
struct Vec2;

#include <string>
#include <vector>

enum TextDrawMode
{
	SHRINK_TO_FIT,
	OVERRUN
};

class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f, int maxGlyphsToDraw = INT_MAX);

	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextDrawMode mode = TextDrawMode::SHRINK_TO_FIT, int maxGlyphsToDraw = INT_MAX);

	void AddVertsForText2DXFoward(std::vector<Vertex_PCU>& verts,
		Vec2 const& textMins, float cellHeight, std::string const& text,
		Rgba8 const& tint = Rgba8::WHITE,
		float cellAspect = 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f),
		int maxGlyphsToDraw = INT_MAX);
	void AddVertsForText3D(std::vector<Vertex_PCU>& verts,
		Vec2 const& textMins, float cellHeight, std::string const& text, 
		Rgba8 const& tint = Rgba8::WHITE, Mat44 modelMatrix = Mat44(),
		float cellAspect = 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f),
		int maxGlyphsToDraw = INT_MAX);
	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f);

protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
};