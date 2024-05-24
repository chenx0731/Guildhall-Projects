#pragma once
#include "Game/SpriteAnimGroupDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include <vector>
#include <string>
#include <map>

class WeaponDefinition
{
public: 
	WeaponDefinition(XmlElement& mapDefElement);
	~WeaponDefinition();
	static void InitializeWeaponDefs(const char* docFilePath);
	static WeaponDefinition const* GetWeaponDef(std::string const& mapDefName);
	static void ClearDefinitions();
public:
	static std::vector<WeaponDefinition*> s_WeaponDefs;

	std::string		m_name;
	float			m_refireTime;
	int				m_rayCount;
	float			m_rayCone;
	float			m_rayRange;
	FloatRange		m_rayDamage;
	float			m_rayImpulse;
	int				m_projectileCount;
	float			m_projectileCone;
	float			m_projectileSpeed;
	std::string		m_projectileActor;
	int				m_meleeCount;
	float			m_meleeRange;
	float			m_meleeArc;
	FloatRange		m_meleeDamage;
	float			m_meleeImpulse;
	bool			m_vacuumToggle = true;

	// HUD
	Shader*									m_shader = nullptr;
	Texture*								m_baseTexture = nullptr;
	Texture*								m_reticleTexture = nullptr;
	IntVec2									m_reticleSize;
	IntVec2									m_spriteSize;
	Vec2									m_spritePivot;
	std::vector<SpriteAnimGroupDefinition>	m_animGroups;
	std::map<std::string, SoundID>			m_audios;
	Texture*								m_demonProjectileTexture = nullptr;
	Texture*								m_soulProjectileTexture = nullptr;
	IntVec2									m_projectileSize;
};
