#pragma once


struct IntVec2
{
public:
	int x = 0;
	int y = 0;

	static const IntVec2 Zero;

public:
	~IntVec2() {}
	IntVec2() {}
	IntVec2(const IntVec2& copyForm);
	explicit IntVec2(int initialX, int initialY);

	float GetLength() const;
	int GetTaxicabLength() const;
	int GetLengthSquared() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;
	IntVec2 const GetRotated90Degrees() const;
	IntVec2 const GetRotatedMinus90Degrees() const;

	void SetFromText(char const* text);

	void Rotate90Degrees();
	void RotateMinus90Degrees();

	void operator=(const IntVec2& copyForm);
	bool		operator==(const IntVec2& compare) const;		
	bool		operator!=(const IntVec2& compare) const;		
	const IntVec2	operator+(const IntVec2& vecToAdd) const;
	const IntVec2	operator-(const IntVec2& vecToSubtract) const;
	bool		operator<(const IntVec2& compare) const;
	bool		operator>(const IntVec2& compare) const;
};