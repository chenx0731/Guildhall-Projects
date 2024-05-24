#pragma once

class ActorUID
{
public:
	ActorUID();
	ActorUID(unsigned int salt, unsigned int index);
	~ActorUID();

	bool operator==(ActorUID const& compare) const;

	int GetIndex();
	bool IsValid();

public:
	unsigned int m_data = INVALID;
	static const unsigned int INVALID;
};