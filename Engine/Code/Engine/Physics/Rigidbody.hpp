#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"


struct AABB2;

enum class RigidBodyType
{
	AABB3,
	Cylinder,
	Sphere,

	NUM
};

struct Rigidbody
{
public:
	RigidBodyType m_type = RigidBodyType::AABB3;
	Vec3 m_position;
	Vec3 m_velocity;
	float m_mass = 0.f;
	EulerAngles m_orientation;
	Vec3 m_angularVelocity;
	Vec3 m_acceleration;
	float m_gravity = 0.f;
	bool m_isMassOn = false;
	bool m_isGravityOn = false;
	bool m_isStable = false;

	// if type is AABB3
	Vec3 m_aabb3Dimensions;
	// if type is CylinderZ
	float m_cylinderHeight = 0.f;
	float m_cylinderRadius = 0.f;
	// if type is Sphere
	float m_sphereRadius = 0.f;

	float m_inertia = 1.f;


public:
	Rigidbody();
	~Rigidbody();
	
	void AddForce(const Vec3& force);
	void AddTorque(const Vec3& torque);
	void AddImpluse(const Vec3& velocity);

	void Update(float deltaSeconds);

	void TurnYawInDirection(const Vec3& direction, float turnDegree);
	bool CheckCollision(Rigidbody& other);

	
};
