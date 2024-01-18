cmake_minimum_required(VERSION 3.17)
project(InvadingTaters)

add_executable(InvadingTaters
        src/main.cpp
        src/data.h
        src/glutil.cpp
        src/glutil.h
        src/drawablehelper.cpp
        src/drawablehelper.h
        src/captracker.h
        src/state_manager.cpp
        src/state_manager.h
        src/color4f.h
	)
	set_property(TARGET InvadingTaters PROPERTY CXX_STANDARD 14)
	target_compile_options(InvadingTaters PRIVATE -Wall)

	# glfw
	add_subdirectory(glfw EXCLUDE_FROM_ALL)
	target_link_libraries(InvadingTaters PRIVATE glfw)

	# glew
	add_definitions(-DGLEW_STATIC)
	add_subdirectory(glew-cmake EXCLUDE_FROM_ALL)
	target_link_libraries(InvadingTaters PRIVATE libglew_static)

	# glm
	add_subdirectory(glm EXCLUDE_FROM_ALL)
	target_link_libraries(InvadingTaters PRIVATE glm)
