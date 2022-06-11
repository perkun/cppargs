workspace "cppargs"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}"

project "cppargs"
	location "./"
	--possible values: ConsoleApp, SharedLib, StaticLib, Makefile, Utility, Packaging
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++17"
	--buildoptions { "-pthread" }

	targetdir ("bin/" .. outputdir)
	objdir ("build/" .. outputdir)


	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
	}

	links {}

	filter "configurations:Debug"
		symbols "On"
		defines "DEBUG"

	filter "configurations:Release"
		-- defines...
		optimize "On"
		defines "RELEASE"

	filter "configurations:Dist"
		-- defines...
		optimize "On"
