#pragma once
#include "Game/SpriteAnimGroupDefinition.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include <vector>
#include <string>
#include <map>
enum class RigidbodyType
{
	AABB3,
	Cylinder,
	Sphere,

	Num
};
class Shader;
class ActorDefinition
{
public: 
	ActorDefinition(XmlElement& actorDefElement);
	~ActorDefinition();
	static void InitializeActorDefs(const char* docFilePath);
	static ActorDefinition const* GetActorDef(std::string const& mapDefName);
	static void ClearDefinitions();
public:
	static std::vector<ActorDefinition*> s_ActorDefs;

	std::string			m_name;
	std::string			m_faction;
	int					m_health;
	bool				m_canBePossessed;
	bool				m_canDamage = true;
	float				m_corpseLifeTime;
	bool				m_isVisible;
	Rgba8				m_solidColor;
	Rgba8				m_wireframeColor;
	bool				m_renderForward;
	bool				m_dieOnSpawn;
	RigidbodyType		m_rigidbodyType = RigidbodyType::Cylinder;

	// collision

	float				m_radius;
	float				m_height;
	bool				m_isCollidesWithWorld;
	bool				m_isCollidesWithActors;
	FloatRange			m_damageOnCollide;
	float				m_impulseOnCollide;
	bool				m_isDieOnCollide;  

	Vec3				m_dimension;

	// physics
	bool				m_isSimulated;
	float				m_walkSpeed;
	float				m_runSpeed;
	float				m_turnSpeed;
	bool				m_isFlying;
	float				m_drag;

	// visual
	Vec2				m_size;
	Vec2				m_pivot;
	std::string			m_billBoardType;
	bool				m_renderLit = false;
	bool				m_renderRounded = false;
	Shader*				m_shader;
	SpriteSheet*		m_spriteSheet;
	IntVec2				m_cellCount;
	IntVec2				m_floorSpriteCoords = IntVec2(-1, -1);
	IntVec2				m_ceilingSpriteCoords = IntVec2(-1, -1);
	IntVec2				m_wallSpriteCoords = IntVec2(-1, -1);
	std::vector<SpriteAnimGroupDefinition*> m_animGroups;
	// Sound
	std::map<std::string, SoundPlaybackID> m_audios;

	// camera
	float			m_eyeHeight;
	float			m_cameraFOV;

	// AI
	bool			m_aiEnabled;
	float			m_sightRadius;
	float			m_sightAngle;

	// inventory
	Strings			m_weapons;
};
