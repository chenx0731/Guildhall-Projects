#pragma once
//#include "Engine/Core/EngineCommon.hpp"
#include <string>

class HashedCaseInsensitiveString
{
public:
	HashedCaseInsensitiveString() = default;
	HashedCaseInsensitiveString(HashedCaseInsensitiveString const& copyFrom) = default;
	HashedCaseInsensitiveString(std::string const& text);
	HashedCaseInsensitiveString(char const* text);

	static unsigned int		CalcHashForText(char const* text);
	static unsigned int		CalcHashForText(std::string const& text);

	bool operator < (HashedCaseInsensitiveString const& comp) const;
	bool operator == (HashedCaseInsensitiveString const& comp) const;
	bool operator != (HashedCaseInsensitiveString const& comp) const;
	bool operator == (std::string const& text) const;
	bool operator != (std::string const& text) const;
	bool operator == (char const* text) const;
	bool operator != (char const* text) const;
	void operator = (HashedCaseInsensitiveString const& assignFrom);
	void operator = (std::string const& assignFrom);
	void operator = (char const* assignFrom);

	std::string GetOriginalText() const;
private:
	std::string		m_originalText;
	unsigned int	m_hash = 0;
};

typedef HashedCaseInsensitiveString HCIString;