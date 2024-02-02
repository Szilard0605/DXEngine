include "ThirdParty/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "DXEngine"
	architecture "x64"
    	startproject "Sandbox"

	configurations { "Debug", "Release", "Dist" }
	
	flags 
    	{ 
		"MultiProcessorCompile" 
    	}

    	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies" 
	include "Dependencies/imgui"
	include "Dependencies/assimp"
group ""

include "DXEngine"
