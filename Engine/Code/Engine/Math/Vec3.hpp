#pragma once
//-----------------------------------------------------------------------------------------------
struct Vec2;
struct Vec3
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
public:
	// Construction/Destruction 											// destructor (do nothing)
	Vec3() = default;												// default constructor (do nothing)
	Vec3(const Vec3& copyFrom);							// copy constructor (from another vec3)
	Vec3(const Vec2& copyFrom, float Z = 0.f);

	explicit Vec3(float initialX, float initialY, float initialZ);		// explicit constructor (from x, y, z)
	//explicit Vec3(int X, int Y, int Z);

	void SetFromText(char const* text, char const* split = ",");

	const static Vec3 MakeFromPolarRadians(float latitudeRadians, float longtitudeRadians, float length = 1.f);
	const static Vec3 MakeFromPolarDegrees(float latitudeDegrees, float longtitudeDegrees, float length = 1.f);

	// Accessors (const methods)
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZRadians() const;
	float GetAngleAboutZDegrees() const;
	Vec3 const GetRotatedAboutZRadians(float deltaRadians) const;
	Vec3 const GetRotatedAboutZDegrees(float deltaDegrees) const;
	Vec3 const GetClamped(float maxLength) const;
	Vec3 const GetNormalized() const;
	Vec2 GetVec2XY() const;
	void GetFloat4(float* outResult) const;

	// Operators (const)
	bool operator==(Vec3 const& compare) const;
	bool operator!=(Vec3 const& compare) const;
	Vec3 const operator+(Vec3 const& vecToAdd) const;
	Vec3 const operator-(Vec3 const& vecToSubtract) const;
	const Vec3	operator-() const;
	Vec3 const operator*(float uniformScale) const;
	Vec3 const operator/(float inverseScale) const;

	void Normalize();

	// Operator (self-mutating / non-const)
	void operator+=(Vec3 const& vecToAdd);
	void operator-=(Vec3 const& vecToSubtract);
	void operator*=(float uniformScale);
	void operator/=(float uniformDivisor);
	void operator=(Vec3 const& copyFrom);

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend Vec3 const operator*(float uniformScale, Vec3 const& vecToScale);
	bool		operator<(const Vec3& compare) const;
};


