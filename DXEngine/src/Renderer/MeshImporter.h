#pragma once

#include "Mesh.h"

class MeshImporter
{
public:
	static std::vector<Mesh> ImportDynamicMesh(const std::filesystem::path& path);

};

