#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const Rgba8 Rgba8::WHITE	= Rgba8(255, 255, 255);
const Rgba8 Rgba8::RED		= Rgba8(255, 0,	0);
const Rgba8 Rgba8::GREEN	= Rgba8(0, 255, 0);
const Rgba8 Rgba8::BLUE		= Rgba8(0, 0, 255);
const Rgba8 Rgba8::YELLOW	= Rgba8(255, 255, 0);
const Rgba8 Rgba8::BLACK	= Rgba8(0, 0, 0);
const Rgba8 Rgba8::GREY		= Rgba8(127, 127, 127);
const Rgba8 Rgba8::LUCID	= Rgba8(0, 0, 0, 0);

Rgba8::Rgba8()
{
	r = 255;
	g = 255;
	b = 255;
	a = 255;
}

Rgba8::Rgba8(unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA)
{
	r = initialR;
	g = initialG;
	b = initialB;
	a = initialA;
}

void Rgba8::SetFromText(char const* text, char const* split)
{
	Strings strings = SplitStringOnDelimiter(text, split);
	if (strings.size() < 3)
		ERROR_AND_DIE("Lack of variables when set Rgba8");
	std::string stringR = strings[0];
	std::string stringG = strings[1];
	std::string stringB = strings[2];
	std::string stringA = "255";
	if (strings.size() == 4)
		stringA = strings[3];
	r = static_cast<unsigned char>(atoi(stringR.c_str()));
	g = static_cast<unsigned char>(atoi(stringG.c_str()));
	b = static_cast<unsigned char>(atoi(stringB.c_str()));
	a = static_cast<unsigned char>(atoi(stringA.c_str()));
}

void Rgba8::GetAsFloats(float* colorAsFloats) const
{
	colorAsFloats[0] = static_cast<float>(r) / 255.f;
	colorAsFloats[1] = static_cast<float>(g) / 255.f;
	colorAsFloats[2] = static_cast<float>(b) / 255.f;
	colorAsFloats[3] = static_cast<float>(a) / 255.f;
}

void Rgba8::SetAsFloats(float* colorAsFloats)
{
	r = DenormalizeByte(colorAsFloats[0]);
	g = DenormalizeByte(colorAsFloats[1]);
	b = DenormalizeByte(colorAsFloats[2]);
	a = DenormalizeByte(colorAsFloats[3]);
}

Rgba8 Rgba8::GetLighterColor() const
{
	Rgba8 color = *this;
	int R, G, B;
	R = color.r;
	G = color.g;
	B = color.b;
	R += 40;
	G += 40;
	B += 40;
	//a -= 100;
	if (R > 255)
		R = 255;
	if (G > 255)
		G = 255;
	if (B > 255)
		B = 255;
	color.r = (unsigned char)R;
	color.g = (unsigned char)G;
	color.b = (unsigned char)G;
	return color;
}

Rgba8 Rgba8::GetDarkerColor() const
{
	Rgba8 color = *this;
	int R, G, B;
	R = color.r;
	G = color.g;
	B = color.b;
	R -= 100;
	G -= 100;
	B -= 100;
	//color.a -= 100;
	if (R < 0)
		R = 0;
	if (G < 0)
		G = 0;
	if (B < 0)
		B = 0;
	color.r = (unsigned char)R;
	color.g = (unsigned char)G;
	color.b = (unsigned char)B;
	return color;
}

bool Rgba8::operator==(const Rgba8& compare) const
{
	if (r == compare.r && g == compare.g && b == compare.b && a == compare.a)
	{
		return true;
	}
	return false;
}