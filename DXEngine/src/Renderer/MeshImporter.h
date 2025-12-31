#pragma once

#include "Mesh.h"

#include <filesystem>


class MeshImporter
{
public:
	static std::vector<SharedPtr<Mesh>> ImportDynamicMesh(std::filesystem::path path);

};

