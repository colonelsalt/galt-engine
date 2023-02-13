#pragma once

#include "NameTag.h"

static constexpr int MAP_SIZE = 300;

inline bool StrEq(char* s1, char* s2)
{
	while (*s1 || *s2)
	{
		if (*s1++ != *s2++)
		{
			return false;
		}
	}
	return true;
}

inline void StrCopy(char* src, char* dest)
{
	while (*src)
	{
		*dest++ = *src++;
	}
	*dest = 0;
}


struct HashNode
{
	char Key[MAX_NAME_LENGTH];
	int Value;

	HashNode* p_Next;
};

struct HashMap
{
	HashNode Values[MAP_SIZE];

	uint32_t Hash(char* key);

	bool Contains(char* key);

	void Add(char* key, uint32_t value);

	// If not present in the map, returns -1
	int Get(char* key);
};