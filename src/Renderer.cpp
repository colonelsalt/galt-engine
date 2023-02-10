

void RenderScene(Shader* shader)
{
	uint32_t numMeshes = g_EntityMaster->NumComponents(ComponentType::MESH);
	Mesh* meshes = g_EntityMaster->GetAllComponents<Mesh>();
	for (uint32_t i = 0; i < numMeshes; i++)
	{
		meshes[i].Draw(shader);
	}

	uint32_t numPrimitives = g_EntityMaster->NumComponents(ComponentType::PRIMITIVE);
	Primitive* primitives = g_EntityMaster->GetAllComponents<Primitive>();
	for (uint32_t i = 0; i < numPrimitives; i++)
	{
		primitives[i].Draw();
	}
}