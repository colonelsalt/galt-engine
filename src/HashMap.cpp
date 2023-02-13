#include "HashMap.h"

uint32_t HashMap::Hash(char* key)
{
	uint32_t hash = 37;
	while (*key)
	{
		hash = (hash * 54059) ^ (key[0] * 76963);
		key++;
	}
	return hash % MAP_SIZE;
}

bool HashMap::Contains(char* key)
{
	uint32_t index = Hash(key);
	HashNode* node = &Values[index];
	if (node->Value)
	{
		while (node)
		{
			if (StrEq(key, node->Key))
			{
				return true;
			}
			node = node->p_Next;
		}
	}

	return false;
}

void HashMap::Add(char* key, uint32_t value)
{
	uint32_t index = Hash(key);
	HashNode* node = &Values[index];
	HashNode* prev = nullptr;
	if (node->Value)
	{
		// Collision
		while (node)
		{
			prev = node;
			node = node->p_Next;
		}
		node = (HashNode*)g_Memory->TempAlloc(sizeof(HashNode));
		prev->p_Next = node;
	}
	StrCopy(key, node->Key);
	node->Value = (int)value + 1;
}

int HashMap::Get(char* key)
{
	uint32_t index = Hash(key);
	HashNode* node = &Values[index];

	if (node->Value > 0)
	{
		// More than one value here
		while(node && !StrEq(key, node->Key))
		{
			node = node->p_Next;
		}
	}
	if (!node)
	{
		return -1;
	}
	return node->Value - 1;
}