project "DXEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir)
	debugdir ("%{prj.location}/")
	
	files
	{
		"**.h",
		"**.cpp",
		"src/**.h",
		"src/**.cpp",
		"./Dependencies/glm/include/**.hpp",
		"./Dependencies/glm/include/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.ASSIMP}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.ImGui}"
	}
	
	libdirs
	{
		"%{LibraryDir.GLFW}",
	}

	links
	{
		"ImGui",
		"Assimp",
		"glfw3_64.lib",
		"d3d11.lib",
		"d3dcompiler.lib",
		"dxgi.lib"
	}

	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "DXE_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"glfw3_64.lib"
		}

	filter "configurations:Release"
		defines "DXE_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"glfw3_64.lib"
		}