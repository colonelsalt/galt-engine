#pragma once

// Technically this is MAX_ENTITIES-1, since 0 is not a valid entityId
constexpr uint32_t MAX_ENTITIES = 50;

struct Entity
{
	uint32_t Id;

	operator uint32_t() const
	{
		return Id;
	}

	template <typename T>
	T* AddComponent();

	template <typename T>
	T* GetComponent();

};