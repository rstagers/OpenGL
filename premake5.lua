-- OpenGL-Sandbox
workspace "OpenGL-Sandbox"
	architecture "x64"
	startproject "OpenGL-Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to OpenGL-Core
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"

-- Projects
group "Dependencies"
	include "OpenGL-Core/vendor/glfw"
	include "OpenGL-Core/vendor/Glad"
	include "OpenGL-Core/vendor/imgui"
group ""

include "OpenGL-Core"
include "OpenGL-Sandbox"

-- OpenGL-Examples
workspace "OpenGL-Examples"
    startproject "OpenGL-Examples"
    architecture "x64"
    startproject "OpenGL-Examples"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to OpenGL-Core
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"

-- Projects
group "Dependencies"
    includeexternal "OpenGL-Core/vendor/glfw"
    includeexternal "OpenGL-Core/vendor/Glad"
    includeexternal "OpenGL-Core/vendor/imgui"
group ""

includeexternal "OpenGL-Core"
include "OpenGL-Examples"

-- This stuff is added to create a new sanddbox!
-- Copy the OpenGL-Sandbox directory to a new
-- directory with the project name.  Then
-- below to end of file copy and add a new section
-- cange the workspace names (see comment)
-- then run scripts/Linux-Premake.sh
-- The Makefile will be created in the new project
-- root directory.

-- OpenGL-FirstTry
workspace "OpenGL-FirstTry"			-- Change
    startproject "OpenGL-FirstTry"	-- Change
    architecture "x64"
    startproject "OpenGL-FirstTry"	-- Change

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to OpenGL-Core
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"

-- Projects
group "Dependencies"
    includeexternal "OpenGL-Core/vendor/glfw"
    includeexternal "OpenGL-Core/vendor/Glad"
    includeexternal "OpenGL-Core/vendor/imgui"
group ""

includeexternal "OpenGL-Core"
include "OpenGL-FirstTry"  -- Change
