#pragma once

constexpr uint32_t MAX_NAME_LENGTH = 60;

struct NameTag : Component
{
	COMPONENT_DEF(ComponentType::NAME_TAG, MAX_NAME_TAGS);

	char Tag[MAX_NAME_LENGTH];

	inline void Init(char* s)
	{
		Assert(!(*Tag));
		int i = 0;
		for (i; s[i]; i++)
		{
			Tag[i] = s[i];
		}
		Tag[i++] = 0;
		Assert(i < MAX_NAME_LENGTH);
	}

};