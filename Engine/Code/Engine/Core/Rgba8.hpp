#pragma once
struct Rgba8 // PoD("Plain Old Data")
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;
	static const Rgba8 BLACK;
	static const Rgba8 GREY;
	static const Rgba8 LUCID;

public:
	Rgba8();
	explicit Rgba8(unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA = 255);
	void SetFromText(char const* text, char const* split);
	void GetAsFloats(float* colorAsFloats) const;
	void SetAsFloats(float* colorAsFloats);
	Rgba8 GetLighterColor() const;
	Rgba8 GetDarkerColor() const;

	bool		operator==(const Rgba8& compare) const;
};