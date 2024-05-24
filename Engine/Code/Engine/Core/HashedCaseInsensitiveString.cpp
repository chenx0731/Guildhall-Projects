#include "Engine/Core/HashedCaseInsensitiveString.hpp"



HashedCaseInsensitiveString::HashedCaseInsensitiveString(std::string const& text)
	: m_originalText(text)
	, m_hash(CalcHashForText(text))
{
}

HashedCaseInsensitiveString::HashedCaseInsensitiveString(char const* text) 
	: m_originalText(text)
	, m_hash(CalcHashForText(text))
{	
}

unsigned int HashedCaseInsensitiveString::CalcHashForText(char const* text)
{
	unsigned int hash = 0;
	char const* readPos = text;
	while (*readPos != '\0')
	{
		hash *= 31;
		hash += (unsigned int)tolower(*readPos);
		++readPos;
	}
	return hash;
}

unsigned int HashedCaseInsensitiveString::CalcHashForText(std::string const& text)
{
	return CalcHashForText(text.c_str());
}

std::string HashedCaseInsensitiveString::GetOriginalText() const
{
	return m_originalText;
}

void HashedCaseInsensitiveString::operator=(char const* assignFrom)
{
	m_originalText = assignFrom;
	m_hash = CalcHashForText(m_originalText);
}

void HashedCaseInsensitiveString::operator=(std::string const& assignFrom)
{
	m_originalText = assignFrom;
	m_hash = CalcHashForText(m_originalText);
}

void HashedCaseInsensitiveString::operator=(HashedCaseInsensitiveString const& assignFrom)
{
	m_originalText = assignFrom.m_originalText;
	m_hash = assignFrom.m_hash;
}

bool HashedCaseInsensitiveString::operator!=(char const* text) const
{
	return _stricmp(m_originalText.c_str(), text) != 0;
}

bool HashedCaseInsensitiveString::operator!=(std::string const& text) const
{
	return _stricmp(m_originalText.c_str(), text.c_str()) != 0;
}

bool HashedCaseInsensitiveString::operator==(std::string const& text) const
{
	return _stricmp(m_originalText.c_str(), text.c_str()) == 0;
}

bool HashedCaseInsensitiveString::operator==(char const* text) const
{
	return _stricmp(m_originalText.c_str(), text) == 0;
}

bool HashedCaseInsensitiveString::operator!=(HashedCaseInsensitiveString const& comp) const
{
	if (m_hash != comp.m_hash)
		return true;
	else
	{
		int res = _stricmp(m_originalText.c_str(), comp.m_originalText.c_str());
		return res != 0;
	}
}

bool HashedCaseInsensitiveString::operator==(HashedCaseInsensitiveString const& comp) const
{
	if (m_hash != comp.m_hash)
		return false;
	else
	{
		int res = _stricmp(m_originalText.c_str(), comp.m_originalText.c_str());
		return res == 0;
	}
}

bool HashedCaseInsensitiveString::operator<(HashedCaseInsensitiveString const& compareRHS) const
{
	if (m_hash < compareRHS.m_hash)
	{
		return true;
	}
	else if (m_hash > compareRHS.m_hash)
	{
		return false;
	}
	else
	{
		// hash values are equal, use an actual string comparison to determine equality
		int res = _stricmp(m_originalText.c_str(), compareRHS.m_originalText.c_str());
		return res < 0;

	}
}

