#define UNUSED(x) (void)(x)
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/StringUtils.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture) : m_fontGlyphsSpriteSheet(fontTexture, IntVec2(16, 16))
{
	m_fontFilePathNameWithNoExtension = fontFilePathNameWithNoExtension;
}

Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, int maxGlyphsToDraw)
{
	int charCount = 0;
	for (int charIndex = 0; charIndex < text.size(); charIndex++) {
		// calcute AABB
		float offsetMinx = static_cast<float>(charIndex) * cellHeight * cellAspect;
		float offsetMaxx = static_cast<float>(charIndex + 1) * cellHeight * cellAspect;
		Vec2 temptextMins = textMins + Vec2(offsetMinx, 0.f);
		Vec2 temptextMaxs = textMins + Vec2(offsetMaxx, cellHeight);
		Vec2 uvMin, uvMax;
		m_fontGlyphsSpriteSheet.GetSpriteUVs(uvMin, uvMax, (int)text[charIndex]);

		if (charCount >= maxGlyphsToDraw)
			AddVertsForAABB2D(vertexArray, AABB2(temptextMins, temptextMaxs), Rgba8::GREY, uvMin, uvMax);
		else
			AddVertsForAABB2D(vertexArray, AABB2(temptextMins, temptextMaxs), tint, uvMin, uvMax);
		charCount++;
	}
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, TextDrawMode mode, int maxGlyphsToDraw)
{
	Strings texts = SplitStringOnDelimiter(text, "\n");
	int maxLength = 0;
	for (int textIndex = 0; textIndex < texts.size(); textIndex++) {
		if (texts[textIndex].size() > maxLength)
			maxLength = (int)texts[textIndex].size();
	}
	float heightTotal = cellHeight * (float)texts.size();
	float widthTotal = cellHeight * cellAspect * (float)maxLength;

	float diffHeight = box.GetDimensions().y - heightTotal;

	if (mode == SHRINK_TO_FIT)
	{
		float aspect = widthTotal / heightTotal;
		if (heightTotal > box.GetDimensions().y) {
			heightTotal = box.GetDimensions().y;
			widthTotal = heightTotal * aspect;
		}
		if (widthTotal > box.GetDimensions().x) {
			widthTotal = box.GetDimensions().x;
			heightTotal = widthTotal / aspect;
		}
		cellHeight = heightTotal / (float)texts.size();
	}

	int charCount = 0;
	for (int textIndex = 0; textIndex < texts.size(); textIndex++) {
		float textOffsetWidth = alignment.x * (box.GetDimensions().x - cellHeight * cellAspect * (float)texts[textIndex].size());
		float textOffsetHeight = alignment.y * diffHeight + (float)(texts.size() - textIndex - 1) * cellHeight;
		Vec2 textMins = box.m_mins + Vec2(textOffsetWidth, textOffsetHeight);
		AddVertsForText2D(vertexArray, textMins, cellHeight, texts[textIndex], tint, cellAspect, maxGlyphsToDraw - charCount);
		charCount += (int)texts[textIndex].size();
	}

}

void BitmapFont::AddVertsForText2DXFoward(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, int maxGlyphsToDraw)
{
	int charCount = 0;
	for (int charIndex = 0; charIndex < text.size(); charIndex++) {
		// calcute AABB
		float offsetMinx = static_cast<float>(charIndex) * cellHeight * cellAspect;
		float offsetMaxx = static_cast<float>(charIndex + 1) * cellHeight * cellAspect;
		Vec2 temptextBL = textMins + Vec2(offsetMinx, 0.f);
		Vec2 temptextTR = textMins + Vec2(offsetMaxx, cellHeight);
		Vec2 temptextBR = Vec2(temptextTR.x, temptextBL.y);
		Vec2 temptextTL = Vec2(temptextBL.x, temptextTR.y);
		Vec2 uvMin, uvMax;
		m_fontGlyphsSpriteSheet.GetSpriteUVs(uvMin, uvMax, (int)text[charIndex]);

		if (charCount >= maxGlyphsToDraw)
			AddVertsForQuad3D(verts, temptextBL, temptextBR, temptextTR, temptextTL, Rgba8::GREY, AABB2(uvMin, uvMax));
		else
			AddVertsForQuad3D(verts, temptextBL, temptextBR, temptextTR, temptextTL, tint, AABB2(uvMin, uvMax));
		charCount++;
	}
	AABB2 bound = GetVertexBounds2D(verts);
	Vec2 alignmentPos = Vec2(textMins.x - alignment.x * bound.GetDimensions().x, textMins.y - alignment.y * bound.GetDimensions().y);
	TransformVertexArrayXY3D((int)verts.size(), verts.data(), 1.f, 0.f, alignmentPos);
}

void BitmapFont::AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, Mat44 modelMatrix, float cellAspect, Vec2 const& alignment, int maxGlyphsToDraw)
{
	std::vector<Vertex_PCU> tempVerts;
	AddVertsForText2DXFoward(tempVerts, textMins, cellHeight, text, tint, cellAspect, alignment, maxGlyphsToDraw);
	TransformVertexArray3D(tempVerts, modelMatrix);
	verts.reserve(verts.size() + tempVerts.size());
	for (int vertsIndex = 0; vertsIndex < (int)tempVerts.size(); vertsIndex++) {
		verts.push_back(tempVerts[vertsIndex]);
	}
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect)
{
	float textNum = static_cast<float>(text.size());
	return textNum * cellHeight * cellAspect;
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	return 1.0f;
}
