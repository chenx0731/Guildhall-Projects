#pragma once
#include <unordered_map>

struct IntVec3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

	static const IntVec3 Zero;

public:
	~IntVec3() {}
	IntVec3() {}
	IntVec3(const IntVec3& copyForm);
	explicit IntVec3(int initialX, int initialY, int initialZ);

	float GetLength() const;
	int GetTaxicabLength() const;
	int GetLengthSquared() const;

	void SetFromText(char const* text);


	void operator=(const IntVec3& copyForm);
	bool		operator==(const IntVec3& compare) const;
	//bool		operator()(const IntVec3& _Left, const IntVec3& _Right) const;
	std::size_t	operator()(const IntVec3& k) const;
	bool		operator!=(const IntVec3& compare) const;		
	const IntVec3	operator+(const IntVec3& vecToAdd) const;
	const IntVec3	operator-(const IntVec3& vecToSubtract) const;
	
};

struct IntVec3Hash
{
	std::size_t operator()(const IntVec3& k) const
	{
		const int p1 = 73856093 * k.x;
		const int p2 = 19349663 * k.y;
		const int p3 = 83492791 * k.z;
		return p1 + p2 + p3;
		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:
	}
};




struct IntVec3Equal
{
	bool operator()(const IntVec3& lhs, const IntVec3& rhs) const
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
};