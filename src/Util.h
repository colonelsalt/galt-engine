#pragma once

inline int RoundToInt(float f)
{
	float x = f * 10.0f;
	return (int)x / 10;
}

inline uint32_t SafeTruncateUInt64(uint64_t value)
{
	Assert(value <= 0xFFFFFFFF);
	return (uint32_t)value;
}

inline void Substring(const char* s, int start, int end, char* dest)
{
	for (int i = start; i < end; i++)
	{
		*dest++ = s[i];
	}
	*dest = 0;
}

inline size_t StrLen(const char* s)
{
	int length = 0;
	while (s[length++]);
	return length;
}

inline int LastIndexOf(const char* s, char c)
{
	int lastIndex = -1;
	for (int i = 0; s[i]; i++)
	{
		if (s[i] == c)
		{
			lastIndex = i;
		}
	}
	return lastIndex;
}

inline void CatStr(const char* s1, const char* s2, char* dest)
{
	int resultIndex = 0;
	for (int i = 0; s1[i]; i++)
	{
		dest[resultIndex++] = s1[i];
	}
	for (int i = 0; s2[i]; i++)
	{
		dest[resultIndex++] = s2[i];
	}
	dest[resultIndex] = 0;
}

static float Max(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	return b;
}

static float Abs(float x)
{
	if (x < 0)
	{
		return -x;
	}
	return x;
}
