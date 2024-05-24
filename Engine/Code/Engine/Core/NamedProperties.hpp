#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
struct Rgba8;
struct Vec2;
struct Vec3;
struct IntVec2;

class NamedPropertyBase;
template<typename T>
class NamedPropertyOfType;

class NamedProperties
{
public:
	template<typename T>
	void		SetValue(std::string const& keyName, T const& value);
	template<typename T>
	T			GetValue(std::string const& keyName, T const& defaultValue) const;

private:
	std::map<std::string, NamedPropertyBase*> m_keyValuePairs;
};

template<typename T>
void NamedProperties::SetValue(std::string const& keyName, T const& value)
{
	m_keyValuePairs[keyName] = new NamedPropertyOfType<T>(value);
}



template<typename T>
T NamedProperties::GetValue(std::string const& keyName, T const& defaultValue) const
{
	std::map<std::string, NamedPropertyBase*>::iterator found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		// Key was not found!
		return defaultValue;
	}
	NamedPropertyBase* pointerToVal = found->second;
	NamedPropertyOfType<T>* pointerToT = dynamic_cast<NamedPropertyOfType<T>*>(pointerToVal);
	if (!pointerToT)
	{
		// Key was found, but the value was not the correct type!
		ERROR_RECOVERABLE(Stringf("NamedProperties::GetValue asked for value of th wrong type!"));
		return defaultValue;
	}
	return pointerToT->m_value;
}

class NamedPropertyBase
{
	friend class NamedProperties;
protected:
	//virtual NamedPropertyBase() {}
	virtual ~NamedPropertyBase() {}
};

template<typename T>
class NamedPropertyOfType : public NamedPropertyBase
{
	friend class NamedProperties;
protected:
	NamedPropertyOfType(T const& value);
	T m_value;
};

template<typename T>
NamedPropertyOfType<T>::NamedPropertyOfType(T const& value) : m_value(value)
{
}
