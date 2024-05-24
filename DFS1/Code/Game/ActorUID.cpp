#define UNUSED(x) (void)(x);
#include "Game/ActorUID.hpp"
#include "Game/GameCommon.hpp"

const unsigned int ActorUID::INVALID = 0;

ActorUID::ActorUID(unsigned int salt, unsigned int index)
{
	salt = salt << 16;
	index = index << 16;
	index = index >> 16;
	m_data = salt | index;
}

ActorUID::~ActorUID()
{
}

bool ActorUID::operator==(ActorUID const& compare) const
{
	if (compare.m_data == m_data)
		return true;
	return false;
}

int ActorUID::GetIndex()
{
	unsigned int index = m_data << 16;
	index = index >> 16;
	return index;
}

bool ActorUID::IsValid()
{
	if (m_data != INVALID)
		return true;
	return false;
}

ActorUID::ActorUID()
{
}
