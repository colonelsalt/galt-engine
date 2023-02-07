#include "Entity.h"

void Entity::Draw(Shader* shader)
{
	if (Type != RenderType::NONE)
	{
		shader->Bind();
		shader->SetMat4("u_Model", Transform.WorldSpace());

		if (Type == RenderType::PRIMITIVE)
		{
			Primitive.Draw();
		}
		else if (Type == RenderType::MESH)
		{
			Mesh.Draw(shader);
		}

	}

	for (uint32_t i = 0; i < Transform.NumChildren; i++)
	{
		Entity* child = Transform.a_Children[i]->p_Entity;
		Assert(child);
		child->Draw(shader);
	}
}