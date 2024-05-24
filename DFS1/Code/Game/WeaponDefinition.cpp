#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

std::vector<WeaponDefinition*> WeaponDefinition::s_WeaponDefs;

WeaponDefinition::WeaponDefinition(XmlElement& weaponDefElement)
{
	m_name					= ParseXmlAttribute(weaponDefElement, "name", m_name);
	m_refireTime			= ParseXmlAttribute(weaponDefElement, "refireTime",		m_refireTime	);
	m_rayCount				= ParseXmlAttribute(weaponDefElement, "rayCount",			m_rayCount		);
	m_rayCone				= ParseXmlAttribute(weaponDefElement, "rayCone",			m_rayCone		);
	m_rayRange				= ParseXmlAttribute(weaponDefElement, "rayRange",			m_rayRange		);
	m_rayDamage				= ParseXmlAttribute(weaponDefElement, "rayDamage",			m_rayDamage		);
	m_rayImpulse			= ParseXmlAttribute(weaponDefElement, "rayImpulse",		m_rayImpulse	);
	m_projectileCount		= ParseXmlAttribute(weaponDefElement, "projectileCount",	m_projectileCount);
	m_projectileCone		= ParseXmlAttribute(weaponDefElement, "projectileCone",	m_projectileCone);
	m_projectileSpeed		= ParseXmlAttribute(weaponDefElement, "projectileSpeed",	m_projectileSpeed);
	m_projectileActor		= ParseXmlAttribute(weaponDefElement, "projectileActor",	m_projectileActor);
	m_meleeCount			= ParseXmlAttribute(weaponDefElement, "meleeCount",		m_meleeCount	);
	m_meleeRange			= ParseXmlAttribute(weaponDefElement, "meleeRange",		m_meleeRange	);
	m_meleeArc				= ParseXmlAttribute(weaponDefElement, "meleeArc",			m_meleeArc		);
	m_meleeDamage			= ParseXmlAttribute(weaponDefElement, "meleeDamage",		m_meleeDamage	);
	m_meleeImpulse   		= ParseXmlAttribute(weaponDefElement, "meleeImpulse",		m_meleeImpulse   );

	XmlElement* soundDefElement = nullptr;
	XmlElement* hudDefElement = nullptr;
	XmlElement* tempDefElement = weaponDefElement.FirstChildElement();
	while (tempDefElement) {
		std::string elementName = tempDefElement->Name();
		if (elementName == "Sounds")
			soundDefElement = tempDefElement;
		if (elementName == "HUD")
			hudDefElement = tempDefElement;
		tempDefElement = tempDefElement->NextSiblingElement();
	}

	if (hudDefElement != nullptr) {
		std::string shader	=   ParseXmlAttribute(*hudDefElement, "shader",		"");
		m_shader			=	g_theRenderer->CreateShader(shader.c_str());
		std::string baseTexture	=  ParseXmlAttribute(*hudDefElement, "baseTexture", "");
		Image base			=	Image(baseTexture.c_str());
		m_baseTexture		=	g_theRenderer->CreateTextureFromImage(base);

		std::string reticleTexture	=   ParseXmlAttribute(*hudDefElement, "reticleTexture",		"");
		Image reticle		=	Image(reticleTexture.c_str());
		m_reticleTexture	=	g_theRenderer->CreateTextureFromImage(reticle);
		m_reticleSize		=   ParseXmlAttribute(*hudDefElement, "reticleSize",		m_reticleSize	   );
		m_spriteSize		=   ParseXmlAttribute(*hudDefElement, "spriteSize",		m_spriteSize	   );
		m_spritePivot		=   ParseXmlAttribute(*hudDefElement, "spritePivot",		m_spritePivot	   );
		m_projectileSize	=	ParseXmlAttribute(*hudDefElement, "projectileSize", m_projectileSize);

		std::string demonProjectile = ParseXmlAttribute(*hudDefElement, "demonProjectileTexture", "");
		if (demonProjectile != "") {
			Image demon = Image(demonProjectile.c_str());
			m_demonProjectileTexture = g_theRenderer->CreateTextureFromImage(demon);
		}

		std::string soulProjectile = ParseXmlAttribute(*hudDefElement, "soulProjectileTexture", "");
		if (soulProjectile != "") {
			Image soul = Image(soulProjectile.c_str());
			m_soulProjectileTexture = g_theRenderer->CreateTextureFromImage(soul);
		}

		XmlElement* animationDefs = hudDefElement->FirstChildElement();
		while (animationDefs)
		{
			SpriteAnimGroupDefinition group = SpriteAnimGroupDefinition(*animationDefs);
			m_animGroups.push_back(group);
			animationDefs = animationDefs->NextSiblingElement();
			//SpriteAnimDefinition anime = SpriteAnimDefinition(animationDefs);
		}
		//m_animations		=   ParseXmlAttribute(weaponDefElement, "animations",		m_animations	   );
	}
	if (soundDefElement != nullptr) {
		XmlElement* subSoundDef = soundDefElement->FirstChildElement();
		while (subSoundDef) {
			std::string sound = ParseXmlAttribute(*subSoundDef, "sound", sound);
			std::string name = ParseXmlAttribute(*subSoundDef, "name", name);
			m_audios[sound] = g_theAudio->CreateOrGetSound(name.c_str(), false);
			subSoundDef = subSoundDef->NextSiblingElement();
		}
	}
}

WeaponDefinition::~WeaponDefinition()
{

}

void WeaponDefinition::InitializeWeaponDefs(const char* docFilePath)
{
	XmlDocument weaponDefsDoc;
	weaponDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = weaponDefsDoc.RootElement();
	XmlElement* weaponDefElement = rootElement->FirstChildElement();
	while (weaponDefElement) {
		std::string elementName = weaponDefElement->Name();
		WeaponDefinition* newWeaponDef = new WeaponDefinition(*weaponDefElement);
		s_WeaponDefs.push_back(newWeaponDef);
		weaponDefElement = weaponDefElement->NextSiblingElement();
	}
}

WeaponDefinition const* WeaponDefinition::GetWeaponDef(std::string const& weaponDefName)
{
	for (int weaponIndex = 0; weaponIndex < s_WeaponDefs.size(); weaponIndex++) {
		if (weaponDefName == s_WeaponDefs[weaponIndex]->m_name) {
			return s_WeaponDefs[weaponIndex];
		}
	}
	return nullptr;
}

void WeaponDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_WeaponDefs.size(); defIndex++) {
		delete s_WeaponDefs[defIndex];
		s_WeaponDefs[defIndex] = nullptr;
	}
}
