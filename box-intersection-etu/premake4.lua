box_intersect_files = { gkit_dir .. "/box-intersection-etu/*.cpp", gkit_dir .. "/box-intersection-etu/*.hpp" }

project("box_intersect")
	language "C++"
	kind "ConsoleApp"
	targetdir ( gkit_dir .. "/bin" )
	files ( gkit_files )
	files ( box_intersect_files )
	files { gkit_dir .. "/box-intersection-etu/main.cpp" }
