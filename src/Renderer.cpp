

void RenderScene(Shader* shader)
{
	for (uint32_t i = 0; i < g_EntityMaster->NumMeshes; i++)
	{
		g_EntityMaster->MeshTable[i].Draw(shader);
	}

	for (uint32_t i = 0; i < g_EntityMaster->NumPrimitives; i++)
	{
		g_EntityMaster->PrimitivesTable[i].Draw();
	}
}