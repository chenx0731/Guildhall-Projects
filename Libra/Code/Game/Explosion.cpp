#include "Game/Explosion.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"

Explosion::Explosion(EntityType type ,float lifetime, float size, Map* owner, Vec2 const& startPos, float orientationDegrees) 
	:m_lifeTime(lifetime), m_size(size) 
	, Entity(owner, startPos, orientationDegrees)
{
	m_entityType = type;
	m_anim = g_explosionAnim;
	m_anim->m_playbackType = SpriteAnimPlaybackType::ONCE;
	m_anim->m_secondsPerFrame = m_lifeTime / 25.f;
}

Explosion::~Explosion()
{

}

void Explosion::Update(float deltaSeconds)
{
	m_livingTime += deltaSeconds;
	if (m_livingTime >= m_lifeTime)
		Die();
}

void Explosion::Render() const
{
	std::vector<Vertex_PCU> animVerts;

	AddVertsForAABB2D(animVerts, AABB2(m_position.x - m_size, m_position.y - m_size, m_position.x + m_size, m_position.y + m_size), Rgba8::WHITE, m_anim->GetSpriteDefAtTime(m_livingTime).GetUVs().m_mins, m_anim->GetSpriteDefAtTime(m_livingTime).GetUVs().m_maxs);
	

	g_theRenderer->BindTexture(&m_anim->GetSpriteDefAtTime(m_livingTime).GetTexture());
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);
	g_theRenderer->DrawVertexArray((int)animVerts.size(), animVerts.data());
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
}
