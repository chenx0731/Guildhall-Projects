#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Audio/AudioSystem.hpp"

std::vector<ActorDefinition*> ActorDefinition::s_ActorDefs;

ActorDefinition::ActorDefinition(XmlElement& actorDefElement)
{
	m_name					= ParseXmlAttribute(actorDefElement, "name", m_name);
	m_faction				= ParseXmlAttribute(actorDefElement, "faction", m_faction);
	m_health				= ParseXmlAttribute(actorDefElement, "health", m_health);
	m_canBePossessed		= ParseXmlAttribute(actorDefElement, "canBePossessed", m_canBePossessed);
	m_canDamage				= ParseXmlAttribute(actorDefElement, "canDamage", m_canDamage);
	m_corpseLifeTime		= ParseXmlAttribute(actorDefElement, "corpseLifetime", m_corpseLifeTime);
	m_isVisible				= ParseXmlAttribute(actorDefElement, "visible", m_isVisible);
	m_solidColor			= ParseXmlAttribute(actorDefElement, "solidColor", m_solidColor);
	m_wireframeColor		= ParseXmlAttribute(actorDefElement, "wireframeColor", m_wireframeColor);
	m_renderForward			= ParseXmlAttribute(actorDefElement, "renderForward", m_renderForward);
	m_dieOnSpawn			= ParseXmlAttribute(actorDefElement, "dieOnSpawn", m_dieOnSpawn);
	std::string rigidbody	= ParseXmlAttribute(actorDefElement, "rigidBody", "Cylinder");

	if (rigidbody == "Cylinder")
		m_rigidbodyType = RigidbodyType::Cylinder;
	else if (rigidbody == "AABB3")
		m_rigidbodyType = RigidbodyType::AABB3;
	else if (rigidbody == "Sphere")
		m_rigidbodyType = RigidbodyType::Sphere;

	
	XmlElement* collisionDefElement = nullptr;// = actorDefElement.FirstChildElement();
	XmlElement* physicsDefElement = nullptr;// = actorDefElement.FirstChildElement();
	XmlElement* cameraDefElement = nullptr; // = actorDefElement.FirstChildElement();
	XmlElement* aiDefElement = nullptr; // = actorDefElement.FirstChildElement();
	XmlElement* visualDefElement = nullptr;// = actorDefElement.FirstChildElement();
	XmlElement* soundDefElement = nullptr;// = actorDefElement.FirstChildElement();
	XmlElement* inventoryDefElement = nullptr;// = actorDefElement.FirstChildElement();
	XmlElement* tempDefElement = actorDefElement.FirstChildElement();
	while (tempDefElement) {
		std::string elementName = tempDefElement->Name();
		if (elementName == "Collision")
			collisionDefElement = tempDefElement;
		if (elementName == "Physics")
			physicsDefElement = tempDefElement;
		if (elementName == "Camera")
			cameraDefElement = tempDefElement;
		if (elementName == "AI")
			aiDefElement = tempDefElement;
		if (elementName == "Visuals")
			visualDefElement = tempDefElement;
		if (elementName == "Sounds")
			soundDefElement = tempDefElement;
		if (elementName == "Inventory")
			inventoryDefElement = tempDefElement;
		tempDefElement = tempDefElement->NextSiblingElement();
	}

	if (collisionDefElement != nullptr) {
		m_radius				= ParseXmlAttribute(*collisionDefElement, "radius", m_radius);
		m_height				= ParseXmlAttribute(*collisionDefElement, "height", m_height);
		m_dimension				= ParseXmlAttribute(*collisionDefElement, "dimension", m_dimension);
		m_isCollidesWithWorld	= ParseXmlAttribute(*collisionDefElement, "collidesWithWorld", m_isCollidesWithWorld);
		m_isCollidesWithActors	= ParseXmlAttribute(*collisionDefElement, "collidesWithActors", m_isCollidesWithActors);
		m_damageOnCollide		= ParseXmlAttribute(*collisionDefElement, "damageOnCollide", m_damageOnCollide);
		m_impulseOnCollide		= ParseXmlAttribute(*collisionDefElement, "impulseOnCollide", m_impulseOnCollide);
		m_isDieOnCollide		= ParseXmlAttribute(*collisionDefElement, "dieOnCollide", m_isDieOnCollide);
	}
	
	if (physicsDefElement != nullptr && physicsDefElement) {
		m_isSimulated	= ParseXmlAttribute(*physicsDefElement, "simulated", m_isSimulated);
		m_walkSpeed		= ParseXmlAttribute(*physicsDefElement, "walkSpeed", m_walkSpeed	);
		m_runSpeed		= ParseXmlAttribute(*physicsDefElement, "runSpeed", m_runSpeed	);
		m_turnSpeed		= ParseXmlAttribute(*physicsDefElement, "turnSpeed", m_turnSpeed	);
		m_isFlying		= ParseXmlAttribute(*physicsDefElement, "flying", m_isFlying);
		m_drag		 	= ParseXmlAttribute(*physicsDefElement, "drag", m_drag);
	}

	if (cameraDefElement != nullptr) {
		m_eyeHeight		= ParseXmlAttribute(*cameraDefElement, "eyeHeight", m_eyeHeight);
		m_cameraFOV		= ParseXmlAttribute(*cameraDefElement, "cameraFOV", m_cameraFOV);
	}

	if (aiDefElement != nullptr) {
		m_aiEnabled = ParseXmlAttribute(*aiDefElement, "aiEnabled", m_aiEnabled);
		m_sightRadius = ParseXmlAttribute(*aiDefElement, "sightRadius", m_sightRadius);
		m_sightAngle = ParseXmlAttribute(*aiDefElement, "sightAngle", m_sightAngle);
	}

	if (visualDefElement != nullptr) {
		m_size				= ParseXmlAttribute(*visualDefElement, "size", m_size);
		m_pivot				= ParseXmlAttribute(*visualDefElement, "pivot", m_pivot);
		m_billBoardType		= ParseXmlAttribute(*visualDefElement, "billboardType", m_billBoardType);
		m_renderLit			= ParseXmlAttribute(*visualDefElement, "renderLit", m_renderLit);
		m_renderRounded		= ParseXmlAttribute(*visualDefElement, "renderRounded", m_renderRounded);
		std::string shader	= ParseXmlAttribute(*visualDefElement, "shader", shader);
		m_shader			= g_theRenderer->CreateShader(shader.c_str());
		// create sprite sheet
		std::string sheet	= ParseXmlAttribute(*visualDefElement, "spriteSheet", sheet);
		m_cellCount			= ParseXmlAttribute(*visualDefElement, "cellCount", m_cellCount);
		m_floorSpriteCoords		= ParseXmlAttribute(*visualDefElement, "floorSpriteCoords", m_floorSpriteCoords);
		m_wallSpriteCoords		= ParseXmlAttribute(*visualDefElement, "wallSpriteCoords", m_wallSpriteCoords);
		m_ceilingSpriteCoords	= ParseXmlAttribute(*visualDefElement, "ceilingSpriteCoords", m_ceilingSpriteCoords);
		Image image			= Image(sheet.c_str());
		Texture* texture	= g_theRenderer->CreateTextureFromImage(image);
		
		m_spriteSheet		= new SpriteSheet(*texture, m_cellCount);

		XmlElement* animeGroupsDef = visualDefElement->FirstChildElement();
		while (animeGroupsDef) {
			SpriteAnimGroupDefinition* temp = new SpriteAnimGroupDefinition(*animeGroupsDef, *m_spriteSheet);
			m_animGroups.push_back(temp);
			animeGroupsDef = animeGroupsDef->NextSiblingElement();
		}

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

	if (inventoryDefElement != nullptr) {
		XmlElement* weaponDefElement = inventoryDefElement->FirstChildElement();
		while (weaponDefElement)
		{
			std::string name = ParseXmlAttribute(*weaponDefElement, "name", "");
			m_weapons.push_back(name);
			weaponDefElement = weaponDefElement->NextSiblingElement();
		}
	}
	/*
	m_isSimulated	= ParseXmlAttribute(*physicsDefElement, "", );
		m_walkSpeed		= ParseXmlAttribute(*physicsDefElement, "", );
		m_runSpeed		= ParseXmlAttribute(*physicsDefElement, "", );
		m_turnSpeed		= ParseXmlAttribute(*physicsDefElement, "", );
		m_drag			= ParseXmlAttribute(*physicsDefElement, "", ); */
}

ActorDefinition::~ActorDefinition()
{

}

void ActorDefinition::InitializeActorDefs(const char* docFilePath)
{
	XmlDocument actorDefsDoc;
	actorDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = actorDefsDoc.RootElement();
	XmlElement* actorDefElement = rootElement->FirstChildElement();
	while (actorDefElement) {
		std::string elementName = actorDefElement->Name();
		ActorDefinition* newActorDef = new ActorDefinition(*actorDefElement);
		s_ActorDefs.push_back(newActorDef);
		actorDefElement = actorDefElement->NextSiblingElement();
	}
}

ActorDefinition const* ActorDefinition::GetActorDef(std::string const& actorDefName)
{
	for (int actorIndex = 0; actorIndex < s_ActorDefs.size(); actorIndex++) {
		if (actorDefName == s_ActorDefs[actorIndex]->m_name) {
			return s_ActorDefs[actorIndex];
		}
	}
	return nullptr;
}

void ActorDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_ActorDefs.size(); defIndex++) {
		delete s_ActorDefs[defIndex];
		s_ActorDefs[defIndex] = nullptr;
	}
}
