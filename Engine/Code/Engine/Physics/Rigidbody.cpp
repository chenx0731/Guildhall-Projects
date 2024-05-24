#define UNUSED(x) (void)(x); 
#include "Engine/Physics/RigidBody.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"


Rigidbody::Rigidbody()
{

}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::AddForce(const Vec3& force)
{
	if (m_isMassOn) {
		if (m_mass != 0.f)
		m_acceleration += force / m_mass;
	}
	else {
		m_acceleration += force;
	}
}

void Rigidbody::AddTorque(const Vec3& torque)
{
	m_angularVelocity += torque;
}

void Rigidbody::AddImpluse(const Vec3& velocity)
{
	m_velocity += velocity;
}

void Rigidbody::Update(float deltaSeconds)
{
	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;
}


void Rigidbody::TurnYawInDirection(const Vec3& direction, float turnDegree)
{
	Vec2 orientation = direction.GetVec2XY();
	float orientationDegree = orientation.GetOrientationDegrees();
	m_orientation.m_yawDegrees = GetTurnedTowardDegrees(m_orientation.m_yawDegrees, orientationDegree, turnDegree);
}

bool Rigidbody::CheckCollision(Rigidbody& other)
{
	if (m_type == RigidBodyType::AABB3) {
		if (other.m_type == RigidBodyType::AABB3) {
			Vec3 distance = m_position - other.m_position;
			Vec3 dimensionSum = 0.5f * (m_aabb3Dimensions + other.m_aabb3Dimensions);
			distance = GetAbsoluteVec3(distance);
			if (distance.x >= dimensionSum.x && distance.y >= dimensionSum.y && distance.z >= dimensionSum.z) {
				return false;
			}
			return true;
		}
	}
	return false;
}
