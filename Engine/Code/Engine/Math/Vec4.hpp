#pragma once

struct Vec3;
struct Vec2;
//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	// Construction/Destruction
	~Vec4() {}												// destructor (do nothing)
	Vec4() {}												// default constructor (do nothing)
	Vec4( const Vec4& copyFrom );							// copy constructor (from another vec2)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y)
	explicit Vec4( const Vec3& copyFrom );
	explicit Vec4(const Vec2& copyFrom);
	void SetFromText(char const* text);


	// Accessors (const methods)
	float GetLength() const;
	float GetLengthSquared() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;
	Vec4 const GetRotated90Degrees() const;
	Vec4 const GetRotatedMinus90Degrees() const;
	Vec4 const GetRotatedRadians(float deltaRadians) const;
	Vec4 const GetRotatedDegrees(float deltaDegrees) const;
	Vec4 const GetClamped(float maxLength) const;
	Vec4 const GetNormalized() const;
	Vec4 const GetReflected(const Vec4& platformVec) const;

	// Mutators (non-const methods)
	void SetOrientationRadians(float newOrientationRadians);
	void SetOrientationDegrees(float newOrientationDegrees);
	void SetPolarRadians(float newOrientationRadians, float newLength);
	void SetPolarDegrees(float newOrientationDegrees, float newLength);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateRadians(float deltaRadians);
	void RotateDegrees(float deltaDegrees);
	void SetLength(float newLength);
	void ClampLength(float maxLength);
	void Normalize();
	void Reflect(const Vec4& platformVec);
	float NormalizeAndGetPreviousLength();

	// Operators (const)
	bool		operator==( const Vec4& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec4& compare ) const;		// vec2 != vec2
	const Vec4	operator+( const Vec4& vecToAdd ) const;		// vec2 + vec2
	const Vec4	operator-( const Vec4& vecToSubtract ) const;	// vec2 - vec2
	const Vec4	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec4	operator*( float uniformScale ) const;			// vec2 * float
	const Vec4	operator*( const Vec4& vecToMultiply ) const;	// vec2 * vec2
	const Vec4	operator/( float inverseScale ) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=( const Vec4& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec4& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec4& copyFrom );				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );	// float * vec2
};


