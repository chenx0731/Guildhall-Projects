#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"


void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	auto attribute = element.FirstAttribute();
	while (attribute)
	{
		SetValue(attribute->Name(), attribute->Value());
		attribute = attribute->Next();
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	return it->second;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	if (it->second == "1" || it->second == "true")
		return true;
	else
		return false;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	return atoi(it->second.c_str());
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	return static_cast<float>(atof(it->second.c_str()));
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	return it->second;
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	Rgba8 result = Rgba8();
	result.SetFromText(it->second.c_str(), ",");
	return result;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	Vec2 result = Vec2();
	result.SetFromText(it->second.c_str());
	return result;
}

Vec3 NamedStrings::GetValue(std::string const& keyName, Vec3 const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	Vec3 result = Vec3();
	result.SetFromText(it->second.c_str());
	return result;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);
	if (it == m_keyValuePairs.end())
		return defaultValue;
	IntVec2 result = IntVec2();
	result.SetFromText(it->second.c_str());
	return result;
}
